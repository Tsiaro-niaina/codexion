#include "codexion.h"

static int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL))
		return (1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
	{
		pthread_mutex_destroy(&sim->log_mutex);
		return (1);
	}
	return (0);
}

static int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->p.number_of_coders);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->p.number_of_coders)
	{
		if (dongle_init(&sim->dongles[i]))
		{
			dongles_destroy(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	set_coder(t_sim *sim, int i)
{
	t_coder	*c;

	c = &sim->coders[i];
	c->id = i + 1;
	c->left = &sim->dongles[i];
	c->right = &sim->dongles[(i + 1) % sim->p.number_of_coders];
	c->last_compile_start = 0;
	c->compile_count = 0;
	c->sim = sim;
}

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->p.number_of_coders);
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < sim->p.number_of_coders)
	{
		if (pthread_mutex_init(&sim->coders[i].lock, NULL))
		{
			coders_destroy(sim, i);
			return (1);
		}
		set_coder(sim, i);
		i++;
	}
	return (0);
}

int	sim_init(t_sim *sim, t_params *p)
{
	memset(sim, 0, sizeof(t_sim));
	sim->p = *p;
	if (init_mutexes(sim))
		return (1);
	if (init_dongles(sim) || init_coders(sim))
	{
		if (sim->dongles)
			dongles_destroy(sim, sim->p.number_of_coders);
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->log_mutex);
		return (1);
	}
	return (0);
}