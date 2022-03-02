#include "Parsing.hpp"

int		main(int ac, char **av)
{
	server_config	data;

	parsing(av[1], data);
	return 0;
}
