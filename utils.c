#include "codexion.h"

long	get_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
	{
		printf("Error: gettimeofday() failure");
		return (0L);
	}
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	usleep_precise(long duration, t_table *table)
{
	long	start_time;
	long	remaining_time;

	start_time = get_time();
	while (get_time() - start_time < duration)
	{
		if (get_bool(&table->mutex, &table->end_process))
			break ;
		remaining_time = duration - (get_time() - start_time);
		if (remaining_time > 1000 && start_time)
			usleep((remaining_time * 1000) - 500);
		else
			while (get_time() - start_time < duration)
				;
	}
}
