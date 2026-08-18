#include "codexion.h"

static bool	coder_burntout(t_coder *coder)
{
	long	time_elapsed;
	long	time_to_burnout;

	if (get_bool(&coder->mutex, &coder->finished_compiling))
		return (false);
	if (get_long(&coder->mutex, &coder->last_compile_start) == -1L)
		return (false);
	time_elapsed = get_time()
		- get_long(&coder->mutex, &coder->last_compile_start);
	time_to_burnout = coder->table->time_to_burnout;
	if (time_elapsed > time_to_burnout)
		return (true);
	return (false);
}

void	*monitor_routine(void *v_table)
{
	int		i;
	t_table	*table;

	table = (t_table *)v_table;
	wait_all_coders_ready(table);
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
				if (table->number_of_coders == 1)
					pthread_cond_signal(&table->cond);
			}
		}
		usleep(500);
	}
	return (NULL);
}
