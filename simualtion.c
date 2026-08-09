#include "codexion.h"

static void	compiling(t_coder *coder)
{
	t_mutex	*table_mutex;
	long	*required_compiles;

	table_mutex = &coder->table->mutex;
	required_compiles = &coder->table->number_of_compiles_required;
	
	pthread_mutex_lock(&coder->first_dongle->mutex);
	write_log(coder, DONGLE);
	pthread_mutex_lock(&coder->second_dongle->mutex);
	write_log(coder, DONGLE);
	
	pthread_mutex_lock(&coder->mutex)
	coder->last_meal_time = get_time()
	pthread_mutex_unlock(&coder->mutex)
	coder->compilations_counter++;
	write_log(coder, COMPILING);
	usleep_precise(table->time_to_compile);
	pthread_mutex_unlock(&coder->first_fork);
	pthread_mutex_unlock(&coder->second_fork);
	if (coder->compilations_counter >= get_long(table_mutex, required_compiles))
		coder->finished_compiling = true;
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

	while (get_bool(&table->mutex, &table->end_process))
	{
		if (coder->finished_compiling)
			break ;

		// 1) compiling
		compiling(coder);
	
		// 2) debugging

		// 3) refactoring

	}
	return (NULL);
}

void	start_compilation(t_table *table)
{
	int	i;

	i = -1;
	while (++i < table->number_of_coders)
	{
		if (pthread_create(&table->coders[i].thread_id, NULL, thread_job, &table->coders[i]))
		{
			void_err("Thread creation faliure @ start_compiling()", table);
			break ;
		}
	}
	// create thread : monitor
	table->start_time = get_time();

	pthread_mutex_lock(&table->mutex);
	table->all_coders_ready = true;
	pthread_cond_broadcast(&table->cond);
	pthread_mutex_unlock(&table->mutex);
	while (i > -1)
		if (pthread_join(&table->coders[i--].thread_id, NULL))
			void_err("Thread join failure @ start_compiling()", table);

	table->start_time = get_time(); // will return 0 on error; handle it
	// set bool : end simulation
	// join thread : monitor
}
