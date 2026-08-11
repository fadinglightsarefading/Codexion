#include "codexion.h"

void	write_log(t_coder *coder, t_log log_code)
{
	long	time_elapsed;

	time_elapsed = get_time() - coder->table->start_time;
	if (log_code == DONGLE)
		printf("%ld %d has taken a dongle\n", time_elapsed, coder->id);
	else if (log_code == COMPILING)
		printf("%ld %d is compiling\n", time_elapsed, coder->id);
	else if (log_code == DEBUGGING)
		printf("%ld %d is debugging\n", time_elapsed, coder->id);
	else if (log_code == REFACTORING)
		printf("%ld %d is refactoring\n", time_elapsed, coder->id);
	else if (log_code == BURNTOUT)
		printf("%ld %d has burnt out\n", time_elapsed, coder->id);
}
