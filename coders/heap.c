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