#include "codexion.h"

bool	get_bool(t_mutex *mutex, bool *input_value)
{
	bool	return_value;

	pthread_mutex_lock(mutex);
	return_value = *input_value;
	pthread_mutex_unlock(mutex);
	return (return_value);
}

void	set_bool(t_mutex *mutex, bool *value, bool updated)
{
	pthread_mutex_lock(mutex);
	*value = updated;
	pthread_mutex_unlock(mutex);
}

long	get_long(t_mutex *mutex, long *input_value)
{
	long	return_value;

	pthread_mutex_lock(mutex);
	return_value = *input_value;
	pthread_mutex_unlock(mutex);
	return (return_value);
}

void	set_long(t_mutex *mutex, long *value, long updated)
{
	pthread_mutex_lock(mutex);
	*value = updated;
	pthread_mutex_unlock(mutex);
}
