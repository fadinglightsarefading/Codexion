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

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}			t_list;

typedef struct s_dongle
{
	t_mutex	mutex;
	t_mutex	scheduler_mutex; // TODO add to clean()
	bool	mutex_init;
	bool	scheduler_mutex_init;
	int	dongle_id;
	t_list		*queue;
}	t_dongle;

typedef	struct s_table	t_table;

typedef struct s_coder
{
	int	id;
	long	compilations_counter;
	long	last_compile_start;	// last compile;
	bool	finished_compiling;
	bool		mutex_init;
	bool		cond_init;
	t_mutex		mutex;
	t_cond		cond;		// TODO add to clean()
	t_table		*table;
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
	pthread_t	monitor;
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
bool	initialise_data(t_table *table);

// simulation.c
void	start_compilation(t_table *table);

// monitor.c
void	*monitor_routine(void *v_table);

// error.c
t_scheduler	scheduler_err(const char *message);
int	int_err(const char *message, bool *error);
char	*null_err(const char *message, t_table *table);
void	void_err(const char *message, t_table *table);

// utils.c
long	get_time(void);
void	usleep_precise(long duration, t_table *table);
void	clean(t_table *table);

// getters_setters.c
bool	get_bool(t_mutex *mutex, bool *input_value);
void	set_bool(t_mutex *mutex, bool *value, bool updated);
long	get_long(t_mutex *mutex, long *input_value);
void	set_long(t_mutex *mutex, long *value, long updated);

// log.c
void	write_log(t_coder *coder, t_log log_code);

// linked_list.c
t_list	*ft_lstnew(void *content);
t_list	*ft_lstlast(t_list *lst);
void	ft_lstadd_back(t_list **lst, t_list *new);

// queue.c
int	enter_queue(t_coder *coder, t_dongle *dongle);
void	wait_queue(t_coder *coder, t_dongle *dongle);
void	update_queue(t_dongle *dongle);

#endif
