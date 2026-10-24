*This project has been created as part of the 42 curriculum by rmandimb.*

# Codexion

## Description
Simulation of coders sharing USB dongles, written in C with POSIX threads.
Each coder needs two dongles to compile and must never burn out.
Dongles are granted according to a FIFO or EDF scheduler, with a cooldown
after each release.

## Instructions

```bash
make
make clean  #remove object files
make fclean #remove the executable file
make re     #recompile everything
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler(fifo|edf)
```

## Blocking cases handled
- Deadlock: dongles are always taken in address order, which breaks the
  circular wait condition (Coffman).
- Starvation: per-dongle priority queue (min-heap). FIFO uses an arrival
  ticket; EDF uses last_compile_start + time_to_burnout, ties broken by id.
- Cooldown: a dongle stores its availability time; the first waiter sleeps
  with pthread_cond_timedwait until it elapses.
- Precise burnout detection: monitor thread polling every 1 ms.
- Log serialization: one mutex around every printf; the stop flag is
  checked under the same mutex so nothing prints after "burned out".
- Single coder: only one dongle exists, the coder can never compile,
  the monitor reports the burnout.

## Thread synchronization mechanisms
- pthread_mutex_t per dongle (state + heap), per coder (last_compile_start,
  compile_count), plus stop_mutex and log_mutex.
- pthread_cond_t per dongle: waiting coders block on it; release and
  stop broadcast on it.
- Lock order, never inverted: dongle mutex -> stop_mutex,
  log_mutex -> stop_mutex. The monitor never holds two locks at once.
- Example: the monitor reads a coder's counters under c->lock while the
  coder updates them under the same lock, so no data race.

## Resources
- POSIX threads man pages: https://man7.org/linux/man-pages/man7/pthreads.7.html?ref=hackernoon.com
- pthread_cond_timedwait(3): https://linux.die.net/man/3/pthread_cond_timedwait
- Coffman conditions: https://faq.computersciencewiki.org/index.php/home/article/coffman-conditions
- deadlock: https://en.wikipedia.org/wiki/Deadlock_(computer_science)
- min-heap: https://docs.zephyrproject.org/latest/kernel/data_structures/min_heap.html
- priority queue references: https://www.geeksforgeeks.org/c/c-program-to-implement-priority-queue

## AI usage: 
- Helped to debug my code at some point