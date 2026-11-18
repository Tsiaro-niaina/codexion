/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmandimb <rmandimb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/17 09:42:03 by rmandimb          #+#    #+#             */
/*   Updated: 2026/11/17 09:43:55 by rmandimb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
