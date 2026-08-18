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
	clean(&table);
	return (0);
}
