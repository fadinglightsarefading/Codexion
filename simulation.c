#include "codexion.h"

static void	debugging_refactoring(t_table *table, t_coder *coder)
{
	if (get_bool(&table->mutex, &table->end_process))
		return ;
	write_log(coder, DEBUGGING);
	usleep_precise(table->time_to_debug, table);
	if (get_bool(&table->mutex, &table->end_process))
		return ;
	write_log(coder, REFACTORING);
	usleep_precise(table->time_to_refactor, table);
}

static void	compiling(t_table *table, t_coder *coder)
{
	if (wait_dongle_available(coder, coder->first_dongle, coder->second_dongle))
		return ;
	if (quit_queue_failsafe(table, coder))
		return ;
	if (acquire_dongles(coder, coder->first_dongle, coder->second_dongle))
		return ;
	set_long(&coder->mutex, &coder->last_compile_start, get_time());
	coder->compilations_counter++;
	write_log(coder, COMPILING);
	usleep_precise(table->time_to_compile, table);
	update_queue(coder->first_dongle);
	coder->first_dongle->last_used_time = get_time();
	pthread_mutex_unlock(&coder->first_dongle->mutex);
	update_queue(coder->second_dongle);
	coder->second_dongle->last_used_time = get_time();
	pthread_mutex_unlock(&coder->second_dongle->mutex);
	if (coder->compilations_counter
		>= get_long(&table->mutex, &table->number_of_compiles_required))
		set_bool(&coder->mutex, &coder->finished_compiling, true);
}

void	*thread_job(void *v_coder)
{
	t_coder	*coder;
	t_table	*table;

	coder = (t_coder *)v_coder;
	table = coder->table;
	pthread_mutex_lock(&table->mutex);
	while (table->all_coders_ready == false)
		pthread_cond_wait(&table->cond, &table->mutex);
	pthread_mutex_unlock(&table->mutex);
	if (coder->id % 2 == 0)
		usleep(START_DELAY_MCS);
	if (table->number_of_coders == 1)
		set_long(&coder->mutex, &coder->last_compile_start, get_time());
	while (!get_bool(&table->mutex, &table->end_process))
	{
		if (get_bool(&coder->mutex, &coder->finished_compiling))
			break ;
		compiling(table, coder);
		debugging_refactoring(table, coder);
	}
	return (NULL);
}

void	start_compilation(t_table *table)
{
	int	i;
	int	j;

	i = -1;
	while (++i < table->number_of_coders)
	{
		if (pthread_create(&table->coders[i].thread_id, NULL, thread_job,
				&table->coders[i]))
		{
			void_err("Thread creation faliure @ start_compiling()", table);
			break ;
		}
	}
	if (pthread_create(&table->monitor, NULL, monitor_routine, table))
		void_err("Thread creation failure @ start_compiling()", table);
	set_long(&table->mutex, &table->start_time, get_time());
	wait_all_coders_ready(table);
	j = -1;
	while (++j < i)
		if (pthread_join(table->coders[j].thread_id, NULL))
			void_err("Thread join failure @ start_compiling()", table);
	set_bool(&table->mutex, &table->end_process, true);
	if (pthread_join(table->monitor, NULL))
		void_err("Thread join failure @ start_compiling()", table);
}
