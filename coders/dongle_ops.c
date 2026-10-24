#include "codexion.h"

static void	timed_wait(t_dongle *d, long at_ms)
{
	struct timespec	ts;

	ts.tv_sec = at_ms / 1000;
	ts.tv_nsec = (at_ms % 1000) * 1000000L;
	pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
}

static int	wait_turn(t_dongle *d, t_coder *c)
{
	t_node	top;

	while (!is_stopped(c->sim))
	{
		heap_peek(&d->waiting, &top);
		if (!d->in_use && top.id == c->id)
		{
			if (get_time_ms() >= d->available_at)
				return (0);
			timed_wait(d, d->available_at);
		}
		else
			pthread_cond_wait(&d->cond, &d->mutex);
	}
	return (1);
}

int	dongle_acquire(t_dongle *d, t_coder *c)
{
	t_node	top;
	long	key;

	pthread_mutex_lock(&d->mutex);
	if (c->sim->p.scheduler == FIFO)
		key = d->ticket++;
	else
		key = c->last_compile_start + c->sim->p.time_to_burnout;
	if (heap_push(&d->waiting, key, c->id) || wait_turn(d, c))
	{
		pthread_mutex_unlock(&d->mutex);
		return (1);
	}
	heap_pop(&d->waiting, &top);
	d->in_use = 1;
	pthread_mutex_unlock(&d->mutex);
	return (0);
}

void	dongle_release(t_dongle *d, t_sim *sim)
{
	pthread_mutex_lock(&d->mutex);
	d->in_use = 0;
	d->available_at = get_time_ms() + sim->p.dongle_cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}