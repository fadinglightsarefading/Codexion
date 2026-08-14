#include "codexion.h"

static bool	coder_burntout(t_coder *coder)
{
	long	time_elapsed;
	long	time_to_burnout;

	if (get_bool(&coder->mutex, &coder->finished_compiling))
		return (false);
	if (get_long(&coder->mutex, &coder->last_compile_start) == -1L)
		return (false);
	time_elapsed = get_time() - get_long(&coder->mutex, &coder->last_compile_start);
	time_to_burnout = coder->table->time_to_burnout;
	if (time_elapsed > time_to_burnout)
		return (true);
	/*
	if (time_elapsed > time_to_burnout)
	{
		printf("time elapsed %ld\nburnout time %ld\n", time_elapsed, time_to_burnout);
		return (true);
	}
	*/
	return (false);
}

void	*monitor_routine(void *v_table)
{
	int	i;
	t_table	*table;

	table = (t_table *)v_table;

	pthread_mutex_lock(&table->mutex);
	while (table->all_coders_ready == false)
		pthread_cond_wait(&table->cond, &table->mutex);
	pthread_mutex_unlock(&table->mutex);

	while (!get_bool(&table->mutex, &table->end_process))
	{
		i = -1;
		while (++i < table->number_of_coders
			&& !get_bool(&table->mutex, &table->end_process))
		{
			if (coder_burntout(&table->coders[i]))
			{
				set_bool(&table->mutex, &table->end_process, true);
				write_log(&table->coders[i], BURNTOUT);
			}
		}
		usleep(500);
	}
	return (NULL);
}
