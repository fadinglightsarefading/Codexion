#include "codexion.h"

static t_scheduler	check_scheduler(const char *str)
{
	char		*lowered;
	int			i;
	t_scheduler	scheduler;

	lowered = malloc(sizeof(char) * strlen(str) + 1);
	if (!lowered)
		return (scheduler_err("Malloc failure in lower_case()"));
	i = -1;
	while (str[++i])
	{
		lowered[i] = str[i];
		if (lowered[i] >= 65 && lowered[i] <= 90)
			lowered[i] += 32;
	}
	lowered[i] = '\0';
	if (!strcmp(lowered, "fifo"))
		scheduler = FIFO;
	else if (!strcmp(lowered, "edf"))
		scheduler = EDF;
	else
		scheduler = scheduler_err("Scheduler invalid");
	free(lowered);
	return (scheduler);
}

static const char	*valid_input_2(const char *str)
{
	int	len;

	len = 0;
	while (*str >= '0' && *str <= '9')
	{
		len++;
		str++;
	}
	if (len > 10)
		return (null_err("Arguments must be <=INT_MAX", NULL));
	return (str);
}

static const char	*valid_input(const char *str)
{
	const char	*number;
	const char	*error;

	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	if (*str == '+')
		str++;
	else if (*str == '-')
		return (null_err("Arguments must be positive values", NULL));
	if (*str <= '0' || *str > '9')
		return (null_err("One or more arguments incorrect", NULL));
	number = str;
	error = valid_input_2(str);
	if (error == NULL)
		return (NULL);
	return (number);
}

static long	ft_atol(const char *str, bool *err_flag)
{
	long	num;

	if (*err_flag == 1)
		return (1);
	str = valid_input(str);
	if (str == NULL)
	{
		*err_flag = 1;
		return (0);
	}
	num = 0;
	while (*str >= '0' && *str <= '9')
		num = (num * 10) + (*str++ - 48);
	if (num > INT_MAX)
		return (int_err("Arguments must be <=INT_MAX", err_flag));
	return (num);
}

int	parse_arguments(char *argv[], t_table *table)
{
	t_scheduler	scheduler;
	bool		err_flag;

	scheduler = check_scheduler(argv[8]);
	if (scheduler == INVALID)
		return (1);
	err_flag = false;
	*table = (t_table){
		.number_of_coders = ft_atol(argv[1], &err_flag),
		.time_to_burnout = ft_atol(argv[2], &err_flag),
		.time_to_compile = ft_atol(argv[3], &err_flag),
		.time_to_debug = ft_atol(argv[4], &err_flag),
		.time_to_refactor = ft_atol(argv[5], &err_flag),
		.number_of_compiles_required = ft_atol(argv[6], &err_flag),
		.dongle_cooldown = ft_atol(argv[7], &err_flag),
		.scheduler = scheduler
	};
	if (err_flag)
		return (1);
	return (0);
}
