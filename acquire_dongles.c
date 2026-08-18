#include "codexion.h"

static void	single_coder_in_your_area(t_table *table, t_coder *coder)
{
	pthread_mutex_lock(&coder->first_dongle->mutex);
	write_log(coder, DONGLE);
	pthread_mutex_lock(&table->mutex);
	pthread_cond_wait(&table->cond, &table->mutex);
	pthread_mutex_unlock(&table->mutex);
	pthread_mutex_unlock(&coder->first_dongle->mutex);
}

int	acquire_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (coder->table->number_of_coders == 1)
	{
		single_coder_in_your_area(coder->table, coder);
		return (1);
	}
	if (first->id > second->id)
	{
		pthread_mutex_lock(&first->mutex);
		write_log(coder, DONGLE);
		pthread_mutex_lock(&second->mutex);
		write_log(coder, DONGLE);
	}
	else
	{
		pthread_mutex_lock(&second->mutex);
		write_log(coder, DONGLE);
		pthread_mutex_lock(&first->mutex);
		write_log(coder, DONGLE);
	}
	return (0);
}
