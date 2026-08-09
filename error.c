#include "codexion.h"

t_scheduler	scheduler_err(const char *message)
{
	fprintf(stderr, "Error: %s\n", message);
	return (INVALID);
}

int	int_err(const char *message, bool *error)
{
	if (error != NULL)
		*error = true;
	fprintf(stderr, "Error: %s\n", message);
	return (1);
}

char	*null_err(const char *message, t_table *table)
{
	if (table != NULL)
	{
		pthread_mutex_lock(&table->mutex);
		table->end_process = true;
		pthread_mutex_unlock(&table->mutex);
	}
	fprintf(stderr, "Error: %s\n", message);
	return (NULL);
}

void	void_err(const char *message, t_table *table)
{
	if (table != NULL)
	{
		pthread_mutex_lock(&table->mutex);
		table->end_process = true;
		pthread_mutex_unlock(&table->mutex);
	}
	fprintf(stderr, "Error: %s\n", message);
}
