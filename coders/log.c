#include "codexion.h"

void	log_state(t_coder *c, const char *msg)
{
	t_sim	*sim;

	sim = c->sim;
	pthread_mutex_lock(&sim->log_mutex);
	if (!is_stopped(sim))
		printf("%ld %d %s\n", get_time_ms() - sim->start, c->id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	log_burnout(t_coder *c)
{
	t_sim	*sim;

	sim = c->sim;
	pthread_mutex_lock(&sim->log_mutex);
	if (!is_stopped(sim))
	{
		pthread_mutex_lock(&sim->stop_mutex);
		sim->stop = 1;
		pthread_mutex_unlock(&sim->stop_mutex);
		printf("%ld %d burned out\n", get_time_ms() - sim->start, c->id);
	}
	pthread_mutex_unlock(&sim->log_mutex);
	sim_stop(sim);
}