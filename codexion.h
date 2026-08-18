#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <string.h>
# include <stdbool.h>
# include <limits.h>

# define RESET	"\033[0m"
# define BOLD	"\033[1m"
# define RED	"\033[31m"

# define START_DELAY_MCS 500

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

typedef pthread_mutex_t	t_mutex;
typedef pthread_cond_t	t_cond;

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}			t_list;

typedef struct s_dongle
{
	t_mutex	mutex;
	t_mutex	scheduler_mutex;
	long	last_used_time;
	t_list	*queue;
	int		id;
	bool	mutex_init;
	bool	scheduler_mutex_init;
}	t_dongle;

typedef struct s_table	t_table;

typedef struct s_coder
{
	long		compilations_counter;
	long		last_compile_start;
	t_mutex		mutex;
	t_cond		cond;
	t_table		*table;
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;
	pthread_t	thread_id;
	int			id;
	bool		finished_compiling;
	bool		mutex_init;
	bool		cond_init;
}	t_coder;

typedef struct s_table
{
	long		number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	long		number_of_compiles_required;
	long		dongle_cooldown;
	long		start_time;
	pthread_t	monitor;
	t_dongle	*dongles;
	t_coder		*coders;
	t_mutex		mutex;
	t_cond		cond;
	t_mutex		write_mutex;
	t_scheduler	scheduler;
	bool		end_process;	// a coder gets burnt out
	bool		all_coders_ready;
	bool		mutex_init;
	bool		cond_init;
	bool		write_mutex_init;
}	t_table;

// parse_and_assign.c
int			parse_arguments(char *argv[], t_table *table);

// intialise_data.c
bool		initialise_data(t_table *table);

// simulation.c
void		start_compilation(t_table *table);

// monitor.c
void		*monitor_routine(void *v_table);

// error.c
t_scheduler	scheduler_err(const char *message);
int			int_err(const char *message, bool *error);
char		*null_err(const char *message, t_table *table);
void		void_err(const char *message, t_table *table);

// utils.c
long		get_time(void);
void		usleep_precise(long duration, t_table *table);

// getters_setters.c
bool		get_bool(t_mutex *mutex, bool *input_value);
void		set_bool(t_mutex *mutex, bool *value, bool updated);
long		get_long(t_mutex *mutex, long *input_value);
void		set_long(t_mutex *mutex, long *value, long updated);

// log.c
void		write_log(t_coder *coder, t_log log_code);

// linked_list.c
t_list		*ft_lstnew(void *content);
t_list		*ft_lstlast(t_list *lst);
void		ft_lstadd_back(t_list **lst, t_list *new);

// queue.c
int			enter_queue_fifo(t_coder *coder, t_dongle *dongle);
int			enter_queue_edf(t_coder *coder, t_dongle *dongle);
void		wait_queue(t_coder *coder, t_dongle *dongle);
void		update_queue(t_dongle *dongle);
int			quit_queue_failsafe(t_table *table, t_coder *coder);

// wait.c
void		wait_all_coders_ready(t_table *table);
void		dongle_cooldown(t_coder *coder, t_dongle *dongle);
int			wait_dongle_available(t_coder *coder, t_dongle *first,
				t_dongle *second);

// acquire_dongles.c
int			acquire_dongles(t_coder *coder, t_dongle *first, t_dongle *second);

// clean.c
void		clean(t_table *table);

#endif
