#include "codexion.h"

static void	assign_dongles(t_coder *coder, t_dongle *dongles, int coder_position)
{
	int	num_coders;

	num_coders = coder->table->number_of_coders;
	if (coder->id % 2 == 0)
	{
		coder->first_dongle = &dongles[(coder_position + 1) % num_coders];
		coder->second_dongle = &dongles[coder_position];
	}
	else
	{
		coder->first_dongle = &dongles[coder_position];
		coder->second_dongle = &dongles[(coder_position + 1) % num_coders];
	}
}

static int	initialise_coders(t_table *table)
{
	int	i;
	t_coder	*coder;

	i = -1;
	while (++i < table->number_of_coders)
	{
		coder = &table->coders[i];
		coder->id = i + 1;
		coder->compilations_counter = 0;
		coder->last_compile_start = -1L;
		coder->finished_compiling = false;
		coder->table = table;
		coder->mutex_init = false;
		coder->cond_init = false;
		if (pthread_mutex_init(&coder->mutex, NULL))
			return (int_err("Mutex failure @ initialise_coders()", NULL));
		coder->mutex_init = true;
		if (pthread_cond_init(&coder->cond, NULL))
			return (int_err("Cond failure @ initialise_coders()", NULL));
		coder->cond_init = true;
		assign_dongles(coder, table->dongles, i);
	}
	return (0);
}

static int	init_table_mutex_cond(t_table *table)
{
	table->mutex_init = false;
	table->cond_init = false;
	if (pthread_mutex_init(&table->mutex, NULL))
		return (int_err("Mutex init failure @ initialise_data()", NULL));
	table->mutex_init = true;
	if (pthread_cond_init(&table->cond, NULL))
		return (int_err("Cond init failure @ initialise_data()", NULL));
	table->cond_init = true;
	return (0);
}

static int	initialise_dongles(t_table *table)
{
	int	i;

	i = -1;
	while (++i < table->number_of_coders)
	{
		table->dongles[i].mutex_init = false;
		table->dongles[i].scheduler_mutex_init = false;
		if (pthread_mutex_init(&table->dongles[i].mutex, NULL))
			return (int_err("Mutex failure @ initialise_data()", NULL));
		table->dongles[i].mutex_init = true;
		if (pthread_mutex_init(&table->dongles[i].scheduler_mutex, NULL))
			return (int_err("Mutex failure @ initialise_data()", NULL));
		table->dongles[i].scheduler_mutex_init = true;
		table->dongles[i].dongle_id = i;
		table->dongles[i].queue = NULL;
		table->dongles[i].last_used_time = -1L;
	}
	return (0);
}

bool	initialise_data(t_table *table)
{
	if (init_table_mutex_cond(table))
		return (true);
	table->coders = NULL;
	table->dongles = NULL;
	table->coders = malloc(table->number_of_coders * sizeof(t_coder));
	table->dongles = malloc(table->number_of_coders * sizeof(t_dongle));
	if (table->dongles == NULL)
		return ((bool)int_err("Malloc failure @ initialise_data()", NULL));
	if (initialise_dongles(table))
		return (true);
	if (table->coders != NULL)
		if (initialise_coders(table))
			return (true);
	return (false);
}
