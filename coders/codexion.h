#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <pthread.h>
# include <unistd.h>
# include <time.h>
# include <sys/time.h>

# define FIFO 0
# define EDF 1

typedef struct 	s_params
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles_required;
	int	dongle_cooldown;
	int	scheduler;
}				t_params;

typedef struct 	s_node
{
	long	key;
	int		id;
}				t_node;

typedef struct 	s_heap
{
	t_node	*nodes;
	int		size;
	int		capacity;
}				t_heap;

typedef struct 	s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long			available_at;
	long			ticket;
	int				in_use;
	t_heap			waiting;
}				t_dongle;

typedef struct 	s_coder
{
	int				id;
	pthread_t		thread;
	t_dongle		*left;
	t_dongle		*right;
	pthread_mutex_t	lock;
	long			last_compile_start;
	int				compile_count;
	struct s_sim	*sim;
}				t_coder;

typedef struct	s_sim
{
	t_params		p;
	long			start;
	int				stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor;
}				t_sim;

int		parse_args(int argc, char **argv, t_params *p);
int		heap_init(t_heap *h, int capacity);
void	heap_destroy(t_heap *h);
int		dongle_init(t_dongle *d);
void	dongle_destroy(t_dongle *d);
int		sim_init(t_sim *sim, t_params *p);
void	sim_destroy(t_sim *sim);
void	dongles_destroy(t_sim *sim, int count);
void	coders_destroy(t_sim *sim, int count);
void	heap_sift_up(t_heap *h, int i);
void	heap_sift_down(t_heap *h, int i);
int		heap_push(t_heap *h, long key, int id);
int		heap_pop(t_heap *h, t_node *out);
int		heap_peek(t_heap *h, t_node *out);
long	get_time_ms(void);
int		is_stopped(t_sim *sim);
void	sim_stop(t_sim *sim);
void	sim_sleep(t_sim *sim, long ms);
void	log_state(t_coder *c, const char *msg);
int		dongle_acquire(t_dongle *d, t_coder *c);
void	dongle_release(t_dongle *d, t_sim *sim);
void	*coder_routine(void *arg);
int		sim_run(t_sim *sim);
void	join_coders(t_sim *sim, int count);
void	log_burnout(t_coder *c);
void	*monitor_routine(void *arg);
void	sim_wait(t_sim *sim);

#endif