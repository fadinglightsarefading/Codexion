#include "codexion.h"

void	wait_all_coders_ready(t_table *table)
{
	pthread_mutex_lock(&table->mutex);
	table->all_coders_ready = true;
	pthread_cond_broadcast(&table->cond);
	pthread_mutex_unlock(&table->mutex);
}

void	dongle_cooldown(t_coder *coder, t_dongle *dongle)
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

int	wait_dongle_available(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (coder->table->scheduler == FIFO)
	{
		if (enter_queue_fifo(coder, first))
			return (1);
		if (enter_queue_fifo(coder, second))
			return (1);
	}
	else if (coder->table->scheduler == EDF)
	{
		if (enter_queue_edf(coder, first))
			return (1);
		if (enter_queue_edf(coder, second))
			return (1);
	}
	else
	{
		set_bool(&coder->table->mutex, &coder->table->end_process, true);
		return (1);
	}
	wait_queue(coder, first);
	wait_queue(coder, second);
	dongle_cooldown(coder, first);
	dongle_cooldown(coder, second);
	return (0);
}

void	single_coder_in_your_area(t_table *table, t_dongle *dongle)
{
	pthread_mutex_lock(&table->mutex);
	pthread_cond_wait(&table->cond, &table->mutex);
	pthread_mutex_unlock(&table->mutex);
	pthread_mutex_unlock(&dongle->mutex);
}
