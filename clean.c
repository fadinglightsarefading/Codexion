#include "codexion.h"

static void	free_queue_nodes(t_list	*queue)
{
	t_list	*to_be_freed;

	while (queue)
	{
		to_be_freed = queue;
		queue = queue->next;
		free(to_be_freed);
	}
}

static void	destroy_dongle_primitives(t_table *table, t_dongle *dongles)
{
	int	i;

	if (dongles)
	{
		i = 0;
		while (i < table->number_of_coders && dongles[i].mutex_init)
			pthread_mutex_destroy(&dongles[i++].mutex);
		i = 0;
		while (i < table->number_of_coders && dongles[i].scheduler_mutex_init)
			pthread_mutex_destroy(&dongles[i++].scheduler_mutex);
		i = -1;
		while (++i < table->number_of_coders)
			if (dongles[i].queue)
				free_queue_nodes(dongles[i].queue);
		free(dongles);
	}
}

static void	destroy_coder_primitives(t_table *table, t_coder *coders)
{
	int	i;

	if (coders)
	{
		i = 0;
		while (i < table->number_of_coders && coders[i].mutex_init)
			pthread_mutex_destroy(&table->coders[i++].mutex);
		i = 0;
		while (i < table->number_of_coders && coders[i].cond_init)
			pthread_cond_destroy(&table->coders[i++].cond);
		free(coders);
	}
}

static void	destroy_table_primitives(t_table *table)
{
	if (table->mutex_init)
		pthread_mutex_destroy(&table->mutex);
	if (table->cond_init)
		pthread_cond_destroy(&table->cond);
	if (table->write_mutex_init)
		pthread_mutex_destroy(&table->write_mutex);
}

void	clean(t_table *table)
{
	destroy_dongle_primitives(table, table->dongles);
	destroy_coder_primitives(table, table->coders);
	destroy_table_primitives(table);
}
