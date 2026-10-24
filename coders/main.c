#include "codexion.h"

int	main(int argc, char **argv)
{
	t_params	params;
	t_sim		sim;

	if (parse_args(argc, argv, &params))
		return (1);
	if (sim_init(&sim, &params))
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (1);
	}
	if (sim_run(&sim))
	{
		fprintf(stderr, "Error: thread creation failed\n");
		sim_destroy(&sim);
		return (1);
	}
	sim_wait(&sim);
	sim_destroy(&sim);
	return (0);
}