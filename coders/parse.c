#include "codexion.h"

static int	parse_int(const char *s, int *out)
{
	long	n;
	int		i;

	i = 0;
	if (s[i] == '+')
		i++;
	if (!s[i])
		return (1);
	n = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (1);
		n = n * 10 + (s[i] - '0');
		if (n > INT_MAX)
			return (1);
		i++;
	}
	*out = (int)n;
	return (0);
}

static int	parse_scheduler(const char *s, int *out)
{
	if (strcmp(s, "fifo") == 0)
		*out = FIFO;
	else if (strcmp(s, "edf") == 0)
		*out = EDF;
	else
		return (1);
	return (0);
}

static int	check_ranges(t_params *p)
{
	if (p->number_of_coders < 1 || p->time_to_burnout < 1
		|| p->time_to_compile < 1 || p->number_of_compiles_required < 1)
		return (1);
	return (0);
}

int	parse_args(int argc, char **argv, t_params *p)
{
	if (argc != 9)
	{
		fprintf(stderr, "Usage: %s number_of_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"number_of_compiles_required dongle_cooldown "
			"scheduler(fifo|edf)\n", argv[0]);
		return (1);
	}
	if (parse_int(argv[1], &p->number_of_coders)
		|| parse_int(argv[2], &p->time_to_burnout)
		|| parse_int(argv[3], &p->time_to_compile)
		|| parse_int(argv[4], &p->time_to_debug)
		|| parse_int(argv[5], &p->time_to_refactor)
		|| parse_int(argv[6], &p->number_of_compiles_required)
		|| parse_int(argv[7], &p->dongle_cooldown)
		|| parse_scheduler(argv[8], &p->scheduler)
		|| check_ranges(p))
	{
		fprintf(stderr, "Error: invalid argument\n");
		return (1);
	}
	return (0);
}