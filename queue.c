#include "codexion.h"

int	enter_queue(t_coder *coder, t_dongle *dongle)
{
	t_list	*node;

	node = ft_lstnew(&coder->cond);
	if (!node)
	{
		set_bool(&coder->table->mutex, &coder->table->end_process, true);
		return (int_err("Malloc failure @ wait_queue()", NULL));
	}
	pthread_mutex_lock(&dongle->scheduler_mutex);
	ft_lstadd_back(&dongle->queue, node);
	pthread_mutex_unlock(&dongle->scheduler_mutex);
	return (0);
}

void	wait_queue(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->scheduler_mutex);
	while (dongle->queue->content != &coder->cond)
		pthread_cond_wait(&coder->cond, &dongle->scheduler_mutex);
	pthread_mutex_unlock(&dongle->scheduler_mutex);
}

void	update_queue(t_dongle *dongle)
{
	t_list	*former_head;

	pthread_mutex_lock(&dongle->scheduler_mutex);
	former_head = dongle->queue;
	dongle->queue = dongle->queue->next;
	free(former_head);
	if (dongle->queue)
		pthread_cond_signal((t_cond *)dongle->queue->content);
	pthread_mutex_unlock(&dongle->scheduler_mutex);
}
