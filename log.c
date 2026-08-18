#include "codexion.h"

void	write_log(t_coder *coder, t_log log_code)
{
	t_table	*table;
	long	time_elapsed;

	pthread_mutex_lock(&coder->table->write_mutex);
	table = coder->table;
	if (log_code != BURNTOUT && get_bool(&table->mutex, &table->end_process))
	{
		pthread_mutex_unlock(&coder->table->write_mutex);
		return ;
	}
	time_elapsed = get_time() - coder->table->start_time;
	if (log_code == DONGLE)
		printf("%ld %d has taken a dongle\n", time_elapsed, coder->id);
	else if (log_code == COMPILING)
		printf(BOLD "%ld %d is compiling\n" RESET, time_elapsed, coder->id);
	else if (log_code == DEBUGGING)
		printf("%ld %d is debugging\n", time_elapsed, coder->id);
	else if (log_code == REFACTORING)
		printf("%ld %d is refactoring\n", time_elapsed, coder->id);
	else if (log_code == BURNTOUT)
		printf(RED BOLD "%ld %d has burnt out\n" RESET,
			time_elapsed, coder->id);
	pthread_mutex_unlock(&coder->table->write_mutex);
}
