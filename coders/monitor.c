#include "codexion.h"

static int	check_coder(t_sim *sim, t_coder *c, int *done)
{
	long	last;
	int		count;

	pthread_mutex_lock(&c->lock);
	last = c->last_compile_start;
	count = c->compile_count;
	pthread_mutex_unlock(&c->lock);
	if (get_time_ms() - last >= sim->p.time_to_burnout)
	{
		log_burnout(c);
		return (1);
	}
	if (count >= sim->p.number_of_compiles_required)
		(*done)++;
	return (0);
}

static int	scan(t_sim *sim)
{
	int	i;
	int	done;

	i = 0;
	done = 0;
	while (i < sim->p.number_of_coders)
	{
		if (check_coder(sim, &sim->coders[i], &done))
			return (1);
		i++;
	}
	if (done == sim->p.number_of_coders)
	{
		sim_stop(sim);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_stopped(sim))
	{
		if (scan(sim))
			break ;
		usleep(1000);
	}
	return (NULL);
}
