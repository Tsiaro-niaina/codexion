#include "codexion.h"

static int	node_less(t_node *a, t_node *b)
{
	if (a->key != b->key)
		return (a->key < b->key);
	return (a->id < b->id);
}

static void	swap_nodes(t_node *a, t_node *b)
{
	t_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_sift_up(t_heap *h, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!node_less(&h->nodes[i], &h->nodes[parent]))
			break ;
		swap_nodes(&h->nodes[i], &h->nodes[parent]);
		i = parent;
	}
}

void	heap_sift_down(t_heap *h, int i)
{
	int	min;

	while (1)
	{
		min = i;
		if (2 * i + 1 < h->size
			&& node_less(&h->nodes[2 * i + 1], &h->nodes[min]))
			min = 2 * i + 1;
		if (2 * i + 2 < h->size
			&& node_less(&h->nodes[2 * i + 2], &h->nodes[min]))
			min = 2 * i + 2;
		if (min == i)
			break ;
		swap_nodes(&h->nodes[i], &h->nodes[min]);
		i = min;
	}
}