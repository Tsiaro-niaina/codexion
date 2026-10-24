#include "codexion.h"

int	dongle_init(t_dongle *d)
{
	if (heap_init(&d->waiting, 2))
		return (1);
	if (pthread_mutex_init(&d->mutex, NULL))
	{
		heap_destroy(&d->waiting);
		return (1);
	}
	if (pthread_cond_init(&d->cond, NULL))
	{
		pthread_mutex_destroy(&d->mutex);
		heap_destroy(&d->waiting);
		return (1);
	}
	d->available_at = 0;
	d->ticket = 0;
	d->in_use = 0;
	return (0);
}

void	dongle_destroy(t_dongle *d)
{
	pthread_cond_destroy(&d->cond);
	pthread_mutex_destroy(&d->mutex);
	heap_destroy(&d->waiting);
}