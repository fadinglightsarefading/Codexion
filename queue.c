#include "codexion.h"

int	enter_queue_fifo(t_coder *coder, t_dongle *dongle)
{
	t_list	*node;

	node = ft_lstnew(coder);
	if (!node)
	{
		pthread_mutex_unlock(&dongle->scheduler_mutex);
		set_bool(&coder->table->mutex, &coder->table->end_process, true);
		return (int_err("Malloc failure @ wait_queue()", NULL));
	}
	ft_lstadd_back(&dongle->queue, node);
	pthread_mutex_unlock(&dongle->scheduler_mutex);
	return (0);
}

int	enter_queue_edf(t_coder *coder, t_dongle *dongle)
{
	long	lcs;
	t_list	*next_node;
	t_coder	*in_queue;

	pthread_mutex_lock(&dongle->scheduler_mutex);
	if (!dongle->queue)
		return (enter_queue_fifo(coder, dongle));
	lcs = get_long(&coder->mutex, &coder->last_compile_start);
	while (dongle->queue->next)
	{
		in_queue = (t_coder *)dongle->queue->next->content;
		if (get_long(&in_queue->mutex, &in_queue->last_compile_start) > lcs)
			break ;
		dongle->queue = dongle->queue->next;
	}
	next_node = dongle->queue->next;
	dongle->queue->next = ft_lstnew(coder);
	if (!dongle->queue->next)
	{
		set_bool(&coder->table->mutex, &coder->table->end_process, true);
		return (int_err("Malloc failure @ wait_queue()", NULL));
	}
	dongle->queue->next->next = next_node;
	pthread_mutex_unlock(&dongle->scheduler_mutex);
	return (0);
}

void	wait_queue(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->scheduler_mutex);
	while ((t_coder *)dongle->queue->content != coder)
		pthread_cond_wait(&coder->cond, &dongle->scheduler_mutex);
	pthread_mutex_unlock(&dongle->scheduler_mutex);
}

void	update_queue(t_dongle *dongle)
{
	t_list	*former_head;
	t_coder	*coder;

	pthread_mutex_lock(&dongle->scheduler_mutex);
	former_head = dongle->queue;
	dongle->queue = dongle->queue->next;
	free(former_head);
	if (dongle->queue)
	{
		coder = (t_coder *)dongle->queue->content;
		pthread_mutex_lock(&coder->mutex);
		pthread_cond_signal(&coder->cond);
		pthread_mutex_unlock(&coder->mutex);
	}
	pthread_mutex_unlock(&dongle->scheduler_mutex);
}

int	quit_queue_failsafe(t_table *table, t_coder *coder)
{
	if (get_bool(&table->mutex, &table->end_process)
		|| coder->finished_compiling)
	{
		update_queue(coder->first_dongle);
		update_queue(coder->second_dongle);
		return (1);
	}
	return (0);
}
