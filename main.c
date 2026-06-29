#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_arguments
{
	unsigned int	number_of_coders;
	unsigned int	time_to_burnout;
	unsigned int	time_to_compile;
	unsigned int	time_to_debug;
	unsigned int	time_to_refactor;
	unsigned int	number_of_compiles_required;
	unsigned int	dongle_cooldown;
	char		*scheduler;
}	t_arguments;

int	is_number(char *dig)
{
	while (*dig)
	{
		if (*dig < 48 && *dig > 57)
		{
			printf("Error: arguments must contain positive integers\n");
			return (1);
		}
	dig++;
	}
	return (0);
}

char	*lower_case(const char *str)
{
	char	*lowered;
	int	i;

	lowered = malloc(sizeof(char) * strlen(str) + 1); // FREE THIS SHIT
	i = -1;
	while (str[++i])
	{
		lowered[i] = str[i];
		if (lowered[i] >= 65 && lowered[i] <= 90)
			lowered[i] += 32;
	}
	lowered[i] = '\0';
	return (lowered);
}

int	is_scheduler(char *sch)
{
	if (!strcmp(sch, "fifo") || !strcmp(sch, "edf"))
	{
		printf("Error: incorrect schduler argument\n");
		return (1);
	}
	return (0);
}

int	parse_arguments(int argc, char *argv[], t_arguments *arguments)
{
	int	i;
	char	*scheduler;

	if (argc < 9)
	{
		printf("Error: argment count must be 8\n");
		return (1);
	}
	i = 0;
	while (i < 8)
		if (is_number(argv[i++]))
			return (1);
	scheduler = lower_case(argv[i]);
	if (is_scheduler(scheduler))
		return (1);
	*arguments = (t_arguments){
		.number_of_coders = atoi(argv[1]),
		.time_to_burnout = atoi(argv[2]),
		.time_to_compile = atoi(argv[3]),
		.time_to_debug = atoi(argv[4]),
		.time_to_refactor = atoi(argv[5]),
		.number_of_compiles_required = atoi(argv[6]),
		.dongle_cooldown = atoi(argv[7]),
		.scheduler = scheduler,
	};
	return (0);
}

int	main(int argc, char *argv[])
{
	t_arguments	*args;

	if (parse_arguments(argc, argv, args))
		return (0);
	printf("Number of coders: %d\nTime to burnout: %d\n"
		"Time to compile: %d\nTime to debug: %d\n"
		"Time to refactor: %d\nNo. of compiles required: %d\n"
		"Dongle cooldown %d\nSchduler: %s\n",
		args->number_of_coders, args->time_to_burnout,
		args->time_to_compile, args->time_to_debug,
		args->time_to_refactor, args->number_of_compiles_required,
		args->dongle_cooldown, args->scheduler);
	return (0);
}
