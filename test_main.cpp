#include "Parsing.hpp"

int		main(int ac, char **av)
{
	server_config	data;

	parsing(av[1], data);

	std::cout << data.server[0].location[0].path << std::endl;
	std::cout << data.server[0].location[0].root << std::endl;
	std::cout << data.server[0].error_page.first << std::endl;
	std::cout << data.server[0].error_page.second << std::endl;
	std::cout << data.server[0].location[1].client_max_body_size << std::endl;
	std::cout << data.server[0].location[0].client_max_body_size << std::endl;

	std::cout << data.server[1].server_name[0] << std::endl;
	return 0;
}
