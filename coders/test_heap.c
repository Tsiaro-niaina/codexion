#include "codexion.h"

int	main(void)
{
	t_heap	h;
	t_node	n;

	heap_init(&h, 8);
	heap_push(&h, 5, 1);
	heap_push(&h, 2, 3);
	heap_push(&h, 2, 1);
	heap_push(&h, 9, 2);
	heap_push(&h, 1, 4);
	while (!heap_pop(&h, &n))
		printf("key=%ld id=%d\n", n.key, n.id);
	heap_destroy(&h);
	heap_init(&h, 2);
	printf("push: %d %d %d (attendu 0 0 1)\n", heap_push(&h, 1, 1),
		heap_push(&h, 2, 2), heap_push(&h, 3, 3));
	heap_destroy(&h);
	return (0);
}