#include "codexion.h"

int	heap_push(t_heap *h, long key, int id)
{
	if (h->size >= h->capacity)
		return (1);
	h->nodes[h->size].key = key;
	h->nodes[h->size].id = id;
	h->size++;
	heap_sift_up(h, h->size - 1);
	return (0);
}

int	heap_pop(t_heap *h, t_node *out)
{
	if (h->size == 0)
		return (1);
	*out = h->nodes[0];
	h->size--;
	h->nodes[0] = h->nodes[h->size];
	heap_sift_down(h, 0);
	return (0);
}

int	heap_peek(t_heap *h, t_node *out)
{
	if (h->size == 0)
		return (1);
	*out = h->nodes[0];
	return (0);
}