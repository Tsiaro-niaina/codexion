#include "codexion.h"

void	join_coders(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

static int	start_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->p.number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start;
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]))
		{
			sim_stop(sim);
			join_coders(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

int	sim_run(t_sim *sim)
{
	sim->start = get_time_ms();
	if (start_coders(sim))
		return (1);
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim))
	{
		sim_stop(sim);
		join_coders(sim, sim->p.number_of_coders);
		return (1);
	}
	return (0);
}

void	sim_wait(t_sim *sim)
{
	pthread_join(sim->monitor, NULL);
	join_coders(sim, sim->p.number_of_coders);
}