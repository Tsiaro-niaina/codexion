*This project has been created as part of the 42 curriculum by rmandimb.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads.
Several coders sit around a circular table. Between each pair of neighbours
there is one USB dongle (as many dongles as coders). To compile, a coder must
hold two dongles at the same time, one on each side. After compiling, the
coder debugs, then refactors, then tries to compile again.

The goal is to keep every coder alive. A coder burns out if
`time_to_burnout` milliseconds pass without starting a new compilation
(counted from the start of the previous compilation, or from the start of the
simulation). The simulation ends either when a coder burns out, or when every
coder has compiled at least `number_of_compiles_required` times.

The project focuses on resource sharing between threads: avoiding deadlock and
starvation, arbitrating access with a scheduler (FIFO or EDF), enforcing a
cooldown on released dongles, and detecting burnout precisely.

### Features

- One thread per coder, plus one monitor thread.
- One mutex-protected dongle between each pair of coders.
- Per-dongle waiting queue implemented as a binary min-heap (no standard
  priority queue).
- Two arbitration policies, `fifo` and `edf`.
- Dongle cooldown after each release.
- Burnout detected and logged within a few milliseconds.
- Serialized logs, nothing printed after a burnout.
- No global variables, no memory leaks, strict input validation.

## Instructions

### Build

```bash
make          # build ./codexion
make clean    # remove object files
make fclean   # remove object files and the executable
make re       # full rebuild
```

The project is compiled with `cc -Wall -Wextra -Werror -pthread`.

### Run

```bash
./codexion number_of_coders time_to_burnout time_to_compile \
           time_to_debug time_to_refactor number_of_compiles_required \
           dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | Number of coders, and also of dongles (>= 1) |
| `time_to_burnout` | Max delay in ms between two compile starts (>= 1) |
| `time_to_compile` | Compile duration in ms, two dongles held (>= 1) |
| `time_to_debug` | Debug duration in ms (>= 0) |
| `time_to_refactor` | Refactor duration in ms (>= 0) |
| `number_of_compiles_required` | Stop when every coder reached it (>= 1) |
| `dongle_cooldown` | Delay in ms before a released dongle can be taken again (>= 0) |
| `scheduler` | `fifo` or `edf` |

All arguments are mandatory. Negative numbers, non-integers, values that
overflow an `int`, and any scheduler other than `fifo` or `edf` are rejected
with an error message and a non-zero exit status.

### Log format

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

## Usage examples

```bash
# Comfortable parameters, the simulation ends on its own
./codexion 5 7000 100 200 200 6 800 fifo

# Same idea with the EDF scheduler and a cooldown
./codexion 5 800 200 200 200 3 50 edf

# A single coder has only one dongle, so a burnout is expected
./codexion 1 800 200 200 200 3 0 fifo
```

Excerpt of a run with 3 coders:

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
200 3 has taken a dongle
200 3 has taken a dongle
200 3 is compiling
300 1 is refactoring
```

Each `is compiling` line is always preceded by two `has taken a dongle` lines
from the same coder.

## Project structure

| File | Role |
|---|---|
| `codexion.h` | Structures, constants, prototypes |
| `main.c` | Entry point: parse, init, run, wait, destroy |
| `parse.c` | Strict argument parsing and validation |
| `heap.c`, `heap_utils.c`, `heap_ops.c` | Binary min-heap (init, sift up/down, push, pop, peek) |
| `dongle.c`, `dongle_ops.c` | Dongle init/destroy, acquire and release |
| `sim_init.c`, `sim_destroy.c` | Allocation and cleanup of the whole simulation |
| `sim_run.c` | Thread creation and joining |
| `coder.c` | Coder routine (take dongles, compile, debug, refactor) |
| `monitor.c` | Monitor thread (burnout and end-of-simulation detection) |
| `log.c` | Serialized logging and burnout message |
| `utils.c` | Time in ms, stop flag, interruptible sleep |

## Blocking cases handled

**Deadlock and Coffman's conditions.** A deadlock needs four conditions at
once: mutual exclusion, hold and wait, no preemption, and circular wait.
A dongle can only be held by one coder, so mutual exclusion cannot be removed.
A coder keeps its first dongle while waiting for the second, so hold and wait
is present, and dongles cannot be taken away from a coder. We break the
circular wait: every coder always takes the dongle with the lowest address
first. Since all coders follow the same global order, no cycle of waiting
coders can ever form.

**Starvation.** Each dongle has its own waiting queue, a min-heap. With `fifo`
the key is a per-dongle arrival ticket, so requests are served in arrival
order. With `edf` the key is `last_compile_start + time_to_burnout`, the
coder's burnout deadline, so the most urgent coder is served first. Equal keys
are broken by coder id, which makes the policy fully deterministic.

**Cooldown.** When a dongle is released, it stores `available_at = now +
dongle_cooldown`. The coder at the top of the queue waits with
`pthread_cond_timedwait` until that time instead of polling or using an
imprecise `usleep`.

**Precise burnout detection.** A dedicated monitor thread checks every coder
about every 1 ms and compares `now - last_compile_start` with
`time_to_burnout`. The burnout message is therefore printed well within the
10 ms required.

**Log serialization.** Every message goes through one `log_mutex`, so two
messages never interleave. The stop flag is tested under that same mutex
before printing, and the burnout message sets the flag under it, so no line
can appear after `burned out`.

**Lost wake-ups at shutdown.** `sim_stop` sets the stop flag, then locks each
dongle mutex and broadcasts its condition variable. A coder waiting on a
dongle checks the flag under that same mutex before sleeping, so it can never
miss the stop signal and block forever.

**Single coder.** With one coder there is exactly one dongle, so the coder can
take it but can never compile. It logs one `has taken a dongle`, then waits.
The monitor reports the burnout at `time_to_burnout`.

**Heap overflow.** A dongle is only requested by its two neighbours, and a
coder is in at most one queue at a time, so each heap has a fixed capacity of
2 and never needs reallocation.

## Thread synchronization mechanisms

### Primitives used

| Primitive | Where | Protects |
|---|---|---|
| `pthread_mutex_t` (per dongle) | `t_dongle.mutex` | `in_use`, `available_at`, `ticket`, the waiting heap |
| `pthread_cond_t` (per dongle) | `t_dongle.cond` | Coders waiting for their turn or for the cooldown |
| `pthread_mutex_t` (per coder) | `t_coder.lock` | `last_compile_start`, `compile_count` |
| `pthread_mutex_t` | `sim.stop_mutex` | The `stop` flag |
| `pthread_mutex_t` | `sim.log_mutex` | Output, and the burnout/stop ordering |

### How coders coordinate on a dongle

To take a dongle, a coder locks the dongle mutex, pushes itself in the heap,
and sleeps on the condition variable until it is both at the top of the heap
and the dongle is free. If the cooldown has not elapsed, it sleeps with
`pthread_cond_timedwait` until `available_at`. It then pops itself from the
heap, sets `in_use`, and unlocks. On release, the coder sets `in_use` to 0,
stores the new `available_at`, and broadcasts, so waiting neighbours re-check
their condition. Every wait is inside a `while` loop to handle spurious
wake-ups.

### Coders and monitor communication

The monitor never touches coder state without a lock. It reads
`last_compile_start` and `compile_count` under `c->lock`, while the coder
updates them under the same lock, so there is no data race. The monitor never
holds two locks at once. The only shared decision, "the simulation is over",
goes through the `stop` flag protected by `stop_mutex`, read through
`is_stopped()` and written by `sim_stop()` or `log_burnout()`.

### Lock ordering

Locks are always taken in the same order, so no lock-level deadlock exists.

```
dongle->mutex  ->  stop_mutex
log_mutex      ->  stop_mutex
```

### Race condition examples prevented

- Two coders cannot take the same dongle, since `in_use` and the heap are only
  modified under the dongle mutex.
- A burnout message cannot be followed by another message, because the stop
  flag is set and the message printed in the same `log_mutex` critical
  section.
- A coder cannot sleep through the shutdown, because `stop` is set before the
  broadcast and tested under the dongle mutex before every wait.

## Testing

```bash
valgrind --leak-check=full ./codexion 5 800 200 200 200 3 0 edf
valgrind --tool=helgrind ./codexion 10 800 200 100 100 3 10 edf
./codexion 200 800 200 100 100 3 0 edf
./codexion 1 800 200 200 200 3 0 fifo
```

Checked cases include invalid arguments, a single coder, many coders (200),
large cooldowns, and no output after `burned out`.

## Known limitations

A coder keeps its first dongle while waiting for the second one. This avoids
deadlock but can waste dongle time, so on very tight parameters (a compile
cycle close to `time_to_burnout`) a burnout can happen even if a perfect
schedule exists. EDF guarantees starvation freedom only for parameters that
are feasible with comfortable margins.

## Resources

- POSIX threads: https://man7.org/linux/man-pages/man7/pthreads.7.html
- pthread_cond_timedwait(3): https://linux.die.net/man/3/pthread_cond_timedwait
- Coffman conditions: https://faq.computersciencewiki.org/index.php/home/article/coffman-conditions
- Deadlock: https://en.wikipedia.org/wiki/Deadlock_(computer_science)
- Min-heap: https://docs.zephyrproject.org/latest/kernel/data_structures/min_heap.html
- Priority queue: https://www.geeksforgeeks.org/c/c-program-to-implement-priority-queue

### AI usage

AI (Claude) was used to discuss the architecture, and debbuging the timing between every cycle not to produce burnout everytime
