#include "codexion.h"

int	main(int argc, char *argv[])
{
	t_table	table;

	if (argc != 9)
	{
		printf("Argument count must be 8\n");
		return (1);
	}
	if (parse_arguments(argv, &table))
	 	return (1);
	table.end_process = initialise_data(&table);
	if (table.end_process == false)
		start_compilation(&table);

	/*
	printf("Number of coders: %ld\nTime to burnout: %ld\n"
		"Time to compile: %ld\nTime to debug: %ld\n"
		"Time to refactor: %ld\nNo. of compiles required: %ld\n"
		"Dongle cooldown %ld\nSchduler: %d\n\n",
		table.number_of_coders, table.time_to_burnout,
		table.time_to_compile, table.time_to_debug,
		table.time_to_refactor, table.number_of_compiles_required,
		table.dongle_cooldown, table.scheduler);
	t_coder	*coder;
	for (int i = 0; i < table.number_of_coders; i++)
	{
		coder = &table.coders[i];
		printf("Coder [%d]\tFirst dongle [%d]\n\t\tSecond dongle [%d]\n",
			coder->id, coder->first_dongle->dongle_id,
			coder->second_dongle->dongle_id);
	}
	*/

	clean(&table);
	return (0);
}

// WHERE I LEFT OFF:
//
//	made a fifo simulation, all I need to do is:
//	1) implement dongle cooldown somehow
//	2) implement edf schedule
//	3) try some edge cases
