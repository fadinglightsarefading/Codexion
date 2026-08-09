#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <string.h>

#include <stdbool.h>
#include <limits.h>

typedef enum e_scheduler
{
	FIFO,
	EDF,
	INVALID,
}	t_scheduler;

typedef enum e_pthread_op
{
	INIT,
	LOCK,
	UNLOCK,
	WAIT,
	BROADCAST,
	CREATE,
	JOIN,
}	t_pthread_op;

typedef enum e_log
{
	DONGLE,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	BURNTOUT,
}	t_log;

typedef pthread_mutex_t t_mutex;
typedef pthread_cond_t	t_cond;

typedef struct s_dongle
{
	t_mutex	mutex;
	bool	mutex_init;
	int	dongle_id;
}	t_dongle;

typedef	struct s_table	t_table;

typedef struct s_coder
{
	int	id;
	long	compilations_counter;
	long	last_compile_start;	// time passed since last compile;
	bool	finished_compiling;
	bool		mutex_init;
	t_table		*table;
	t_mutex		mutex;
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;
	pthread_t	thread_id;	// a coder is a thread, fed to *_create();
}	t_coder;

typedef struct s_table
{
	long	number_of_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	long	number_of_compiles_required;
	long	dongle_cooldown;
	t_scheduler	scheduler;
	long		start_time;
	bool		end_process;	// a coder gets burnt out
	bool		all_coders_ready;
	t_dongle	*dongles;	// array to all the dongles
	t_coder		*coders;	// array to all the coders
	t_mutex		mutex;
	t_cond		cond;
	bool		mutex_init;
	bool		cond_init;
}	t_table;

// parse_and_assign.c
int	parse_arguments(char *argv[], t_table *table);

// intialise_data.c
int	initialise_data(t_table *table);

// simulation.c
void	start_compilation(t_table *table);

// error.c
t_scheduler	scheduler_err(const char *message);
int	int_err(const char *message, bool *error);
char	*null_err(const char *message, t_table *table);
void	void_err(const char *message, t_table *table);

// utils.c
long	get_time(void);
bool	end_process(t_table *table);
void	clean(t_table *table);

// getters_setters.c
bool	get_bool(t_mutex *mutex, bool *input_value);
void	set_bool(t_mutex *mutex, bool *value, bool updated);
long	get_long(t_mutex *mutex, long *input_value);
void	set_long(t_mutex *mutex, long *value, long updated);

// log.c
void	write_log(t_coder *coder, t_log log_code);

#endif
