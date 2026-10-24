#include "codexion.h"

static int	take_dongles(t_coder *c)
{
	t_dongle	*first;
	t_dongle	*second;

	first = c->left;
	second = c->right;
	if (c->right < c->left)
	{
		first = c->right;
		second = c->left;
	}
	if (dongle_acquire(first, c))
		return (1);
	log_state(c, "has taken a dongle");
	if (dongle_acquire(second, c))
	{
		dongle_release(first, c->sim);
		return (1);
	}
	log_state(c, "has taken a dongle");
	return (0);
}

static void	do_compile(t_coder *c)
{
	pthread_mutex_lock(&c->lock);
	c->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&c->lock);
	log_state(c, "is compiling");
	sim_sleep(c->sim, c->sim->p.time_to_compile);
	pthread_mutex_lock(&c->lock);
	c->compile_count++;
	pthread_mutex_unlock(&c->lock);
}

static void	solo(t_coder *c)
{
	if (!dongle_acquire(c->left, c))
		log_state(c, "has taken a dongle");
	while (!is_stopped(c->sim))
		usleep(500);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;

	c = (t_coder *)arg;
	if (c->sim->p.number_of_coders == 1)
	{
		solo(c);
		return (NULL);
	}
	while (!take_dongles(c))
	{
		do_compile(c);
		dongle_release(c->left, c->sim);
		dongle_release(c->right, c->sim);
		log_state(c, "is debugging");
		sim_sleep(c->sim, c->sim->p.time_to_debug);
		log_state(c, "is refactoring");
		sim_sleep(c->sim, c->sim->p.time_to_refactor);
	}
	return (NULL);
}