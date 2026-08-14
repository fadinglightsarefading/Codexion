#include "codexion.h"

static void	dongle_cooldown(t_coder *coder, t_dongle *dongle)
{
	long	dongle_cooldown;
	long	last_used_time;
	long	time_elapsed;

	dongle_cooldown = coder->table->dongle_cooldown;
	last_used_time = get_long(&dongle->mutex, &dongle->last_used_time);

	if (last_used_time == -1L)
		return ;

	time_elapsed = get_time() - last_used_time;
	if (time_elapsed < dongle_cooldown)
		usleep_precise(dongle_cooldown - time_elapsed, coder->table);
}

int	wait_dongle_availability(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (enter_queue(coder, first))
		return (1);
	if (enter_queue(coder, second))
		return (1);
	wait_queue(coder, first);
	wait_queue(coder, second);
	dongle_cooldown(coder, first);
	dongle_cooldown(coder, second);
	return (0);
}

static void	compiling(t_coder *coder, t_table *table)
{
	if (wait_dongle_availability(coder, coder->first_dongle, coder->second_dongle))
		return ;
	if (quit_queue_failsafe(table, coder))
		return ;

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
	set_long(&coder->first_dongle->mutex, &coder->first_dongle->last_used_time, get_time());

	update_queue(coder->second_dongle);
	pthread_mutex_unlock(&coder->second_dongle->mutex);
	set_long(&coder->second_dongle->mutex, &coder->second_dongle->last_used_time, get_time());

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
		compiling(coder, table);

		if (get_bool(&table->mutex, &table->end_process))
			break ;
		write_log(coder, DEBUGGING);
		usleep_precise(table->time_to_debug, table);

		if (get_bool(&table->mutex, &table->end_process))
			break ;
		write_log(coder, REFACTORING);
		usleep_precise(table->time_to_refactor, table);

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
