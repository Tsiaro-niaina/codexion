/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmandimb <rmandimb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/17 09:42:35 by rmandimb          #+#    #+#             */
/*   Updated: 2026/11/18 16:42:16 by rmandimb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}

int	is_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	sim_stop(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	i = 0;
	while (i < sim->p.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

void	sim_sleep(t_sim *sim, long ms)
{
	long	end;
	long	left;

	end = get_time_ms() + ms;
	while (!is_stopped(sim))
	{
		left = end - get_time_ms();
		if (left <= 0)
			break ;
		if (left > 1)
			usleep(1000);
		else
			usleep(200);
	}
}
