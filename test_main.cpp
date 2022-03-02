#include "Parsing.hpp"

int		main(int ac, char **av)
{
	server_config	data;

	parsing(av[1], data);

	std::cout << data.server[0].server_name << std::endl;
	return 0;
}
