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
		pthread_mutex_lock(&first->scheduler_mutex);
		if (enter_queue_fifo(coder, first))
			return (1);
		pthread_mutex_lock(&second->scheduler_mutex);
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
		return (42);
	wait_queue(coder, first);
	wait_queue(coder, second);
	dongle_cooldown(coder, first);
	dongle_cooldown(coder, second);
	return (0);
}
