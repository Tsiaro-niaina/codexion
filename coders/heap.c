/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmandimb <rmandimb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/17 09:42:00 by rmandimb          #+#    #+#             */
/*   Updated: 2026/11/17 09:43:40 by rmandimb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *h, int capacity)
{
	h->nodes = malloc(sizeof(t_node) * capacity);
	if (!h->nodes)
		return (1);
	h->size = 0;
	h->capacity = capacity;
	return (0);
}

void	heap_destroy(t_heap *h)
{
	free(h->nodes);
	h->nodes = NULL;
}
