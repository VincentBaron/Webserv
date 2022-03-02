#include "Parsing.hpp"
#include <fstream>

void	pars_server(std::ifstream & conf_file, server_config & data)
{
	server_s	tmp;
	std::string	line;	
	int			pos;

	std::getline(conf_file, line);
	while ((pos = line.find('}')) == std::string::npos) //|| line.find("{", 0, pos) != std::string::npos)
	{
		pos = line.find_first_not_of(" \t");	
		line.erase(0, pos);
		if (line.compare(0, 11, "server_name") == 0)
			std::cout << line << std::endl;
		std::getline(conf_file, line);
	}
}

void	parsing(std::string name, server_config & data)
{
	std::ifstream	conf_file;
	std::string		line;
	int				pos;

	conf_file.open(name);
	if (conf_file.is_open())
	{
		while (conf_file)
		{
			std::getline(conf_file, line);
			if ((pos =line.find("server")) != std::string::npos)
			{
				if (line.substr(pos) == "server {")
					pars_server(conf_file, data);
					/* std::cout << line << std::endl; */
			}
		}
	}
	else
	{
		std::cout << "Could not open file\n";
		exit(1);
	}
}
