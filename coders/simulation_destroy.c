/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simu_destroy.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmandimb <rmandimb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/17 09:42:18 by rmandimb          #+#    #+#             */
/*   Updated: 2026/11/17 09:44:35 by rmandimb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	dongles_destroy(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		dongle_destroy(&sim->dongles[i]);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

void	coders_destroy(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->coders[i].lock);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

void	sim_destroy(t_sim *sim)
{
	coders_destroy(sim, sim->p.number_of_coders);
	dongles_destroy(sim, sim->p.number_of_coders);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
}
