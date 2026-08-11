#include "codexion.h"

static void	refactoring(t_coder *coder, t_table *table)
{
	if (get_bool(&table->mutex, &table->end_process) || coder->finished_compiling)
		return ;
	write_log(coder, REFACTORING);
	usleep_precise(table->time_to_refactor, table);
}

static void	debugging(t_coder *coder, t_table *table)
{
	if (get_bool(&table->mutex, &table->end_process) || coder->finished_compiling)
		return ;
	write_log(coder, DEBUGGING);
	usleep_precise(table->time_to_debug, table);
}

static void	compiling(t_coder *coder, t_table *table)
{
	if (enter_queue(coder, coder->first_dongle))
		return ;
	if (enter_queue(coder, coder->second_dongle))
		return ;
	wait_queue(coder, coder->first_dongle);
	wait_queue(coder, coder->second_dongle);
	
	pthread_mutex_lock(&coder->first_dongle->mutex);
	write_log(coder, DONGLE);
	pthread_mutex_lock(&coder->second_dongle->mutex);
	write_log(coder, DONGLE);

	set_long(&coder->mutex, &coder->last_compile_start, get_time());

	coder->compilations_counter++;
	write_log(coder, COMPILING);
	usleep_precise(table->time_to_compile, table);

	update_queue(coder->first_dongle);
	pthread_mutex_unlock(&coder->first_dongle->mutex);
	update_queue(coder->second_dongle);
	pthread_mutex_unlock(&coder->second_dongle->mutex);

	if (coder->compilations_counter >=
		get_long(&table->mutex, &table->number_of_compiles_required))
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

	while (!get_bool(&table->mutex, &table->end_process))
	{
		if (get_bool(&coder->mutex, &coder->finished_compiling))
			break ;

		// 1) compiling
		compiling(coder, table);
	
		// 2) debugging
		debugging(coder, table);

		// 3) refactoring
		refactoring(coder, table);

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
		if (pthread_create(&table->coders[i].thread_id, NULL, thread_job, &table->coders[i]))
		{
			void_err("Thread creation faliure @ start_compiling()", table);
			break ;
		}
	}
	if (pthread_create(&table->monitor, NULL, monitor_routine, table))
		void_err("Thread creation failure @ start_compiling()", table);

	table->start_time = get_time();

	pthread_mutex_lock(&table->mutex);
	table->all_coders_ready = true;
	pthread_cond_broadcast(&table->cond);
	pthread_mutex_unlock(&table->mutex);
	j = -1;
	while (++j < i)
		if (pthread_join(table->coders[j].thread_id, NULL))
			void_err("Thread join failure @ start_compiling()", table);

	set_bool(&table->mutex, &table->end_process, true); // is this actually necessary?
							    //
	if (pthread_join(table->monitor, NULL))
		void_err("Thread join failure @ start_compiling()", table);
}
