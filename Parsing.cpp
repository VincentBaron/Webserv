#include "Parsing.hpp"
#include <fstream>

std::vector<int>		parse_port(std::string line)
{
	std::vector<int>		ret;
	std::string				delim = " ";
	std::string				port_num;
	int						pos;

	line.erase(0, 6);									//remove "listen" ";" and first spaces and tabs
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error in conf file." << std::endl;
		exit(1);
	}

	line.erase(pos);

	while ((pos = line.find(delim)) != std::string::npos) //split the line and store the numbers
	{
		port_num = line.substr(0, pos);
		ret.push_back(atoi(port_num.c_str()));
		line.erase(0, pos + 1);
	}
	ret.push_back(atoi(line.c_str()));

	return ret;
}

std::string		parse_root(std::string line)
{
	std::string		ret;
	int				pos;

	line.erase(0, 4);
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find_first_of(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error in conf file." << std::endl;
		exit(1);
	}
	ret = line.substr(0, pos);

	return ret;
}

std::string		parse_name(std::string line)
{
	std::string		ret;
	int				pos;

	line.erase(0, 11);                       //remove "server_name and first spaces and tabs"
	pos = line.find_first_not_of(" \t");	
	line.erase(0, pos);
	pos = line.find_first_of(';');
	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error in conf file." << std::endl;
		exit(1);
	}
	ret = line.substr(0, pos);

	return ret;
}

void	pars_server(std::ifstream & conf_file, server_config & data)
{
	server_s	tmp;
	std::vector<int>	tmp_port;
	std::string	line;	
	int			pos;

	std::getline(conf_file, line);
	while ((pos = line.find('}')) == std::string::npos || line.find("{", 0, pos - 1) != std::string::npos)
	{
		pos = line.find_first_not_of(" \t");	
		line.erase(0, pos);
		if (line.compare(0, 11, "server_name") == 0) //parse server_name
			tmp.server_name = parse_name(line);
		else if (line.compare(0, 6, "listen") == 0)  //parse listen
		{
			tmp_port = parse_port(line);

			std::vector<int>::iterator	last = tmp_port.end();
			std::vector<int>::iterator	first = tmp_port.begin();

			for ( ; first != last ; ++first)
				tmp.port.push_back(*first);
		}
		else if (line.compare(0, 4, "root") == 0)   //parse root
			tmp.root = parse_root(line);

		std::getline(conf_file, line);
	}
	
	data.server.push_back(tmp); //add the server to data
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
			}
		}
	}
	else
	{
		std::cout << "Could not open file\n";
		exit(1);
	}
}
