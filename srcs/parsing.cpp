#include "../includes/Parsing.hpp"
#include <fstream>

location_s		parse_location(std::ifstream & conf_file, std::string line)
{
	location_s		ret;
	size_t			pos;

	line.erase(0, 8);			
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find('{');

	if (pos == std::string::npos)              //error if there is not "{"
	{
		std::cout << "Syntax error:" << line << std::endl;
		_exit(1);
	}

	line.erase(pos);									//erase "{" and last espace if it exists	
	if ((pos = line.find(' ')) != std::string::npos)
		line.erase(pos);

	ret.path = line;

	std::getline(conf_file, line);
	while ((pos = line.find('}')) == std::string::npos)
	{
		pos = line.find_first_not_of(" \t");	
		line.erase(0, pos);
		if (line.compare(0, 4, "root") == 0)						//parse root	
			ret.root = parse_root(line);
		else if (line.compare(0, 20, "client_max_body_size") == 0)	//parse max body size
			ret.client_max_body_size = parse_body_size(line);
		else if (line.compare(0, 8, "location") == 0)				//if location inside location: error
		{
			std::cout << "Syntax error:" << line << std::endl;
			_exit(1);
		}
		else if (line.compare(0, 12, "limit_except") == 0)			//parse allowed methods
			ret.allowed_methods = parse_methods(line);
		else if (line.compare(0, 5, "index") == 0) 					//parse index
			ret.index = parse_index(line);
		else if (line.compare(0, 10, "error_page") == 0) 			//parse error_page
			ret.error_page = parse_error_page(line);

		std::getline(conf_file, line);
	}

	return ret;
}

void	parse_server(std::ifstream & conf_file, server_config & data)
{
	server_s	tmp;
	std::string	line;	
	size_t		pos;
	
	char		*pwd = getcwd(NULL, 0);
	if (pwd)
	{
		tmp.root = pwd;
		tmp.root += "/www";
		free(pwd);
	}

	std::getline(conf_file, line);
	while ((pos = line.find('}')) == std::string::npos || line.find("{", 0, pos - 1) != std::string::npos)
	{
		pos = line.find_first_not_of(" \t");	
		line.erase(0, pos);
		if (line.compare(0, 11, "server_name") == 0) 	//parse server_name
		{
			std::vector<std::string>	tmp_name = parse_name(line);

			std::vector<std::string>::iterator	last = tmp_name.end();
			std::vector<std::string>::iterator	first = tmp_name.begin();
			
			for ( ; first != last ; ++first)
				tmp.server_name.push_back(*first);
		}
		else if (line.compare(0, 6, "listen") == 0) 	 //parse listen
		{
			std::vector<int>	tmp_port = parse_port(line);

			std::vector<int>::iterator	last = tmp_port.end();
			std::vector<int>::iterator	first = tmp_port.begin();

			for ( ; first != last ; ++first)
				tmp.port.push_back(*first);
		}
		else if (line.compare(0, 4, "root") == 0)  		 			//parse root
			tmp.root = parse_root(line);
		else if (line.compare(0, 10, "error_page") == 0) 			//parse error_page
			tmp.error_page = parse_error_page(line);
		else if (line.compare(0, 20, "client_max_body_size") == 0)	//parse body_size
			tmp.client_max_body_size = parse_body_size(line);
		else if (line.compare(0, 5, "index") == 0)					//parse_index
			tmp.index = parse_index(line);
		else if (line.compare(0, 12, "limit_except") == 0)			//parse allowed methods
			tmp.allowed_methods = parse_methods(line);
		else if (line.compare(0, 8, "location") == 0)				//parse location
			tmp.location.push_back(parse_location(conf_file, line));

		std::getline(conf_file, line);
	}

	tmp.complete_locations();
	
	data.server.push_back(tmp); //add the server to data
}

void	parsing(int ac, char **av, server_config & data)
{
	std::ifstream	conf_file;
	std::string		line;
	size_t			pos;

	if (ac == 1)
	{
		std::cout << "Need configuration file" << std::endl;
		_exit(1);
	}

	conf_file.open(av[1]);
	if (conf_file.is_open())
	{
		while (conf_file)
		{
			std::getline(conf_file, line);
			if ((pos = line.find("server")) != std::string::npos)
			{
				if (line.substr(pos) == "server {")
					parse_server(conf_file, data);
			}
		}
	}
	else
	{
		std::cout << "Could not open file\n";
		_exit(1);
	}
}

const std::vector<std::string>	server_config::get_allowed_methods(int s, int l) const
{
	if (l == -1)
		return server[s].allowed_methods;
	else
		return server[s].location[l].allowed_methods;
}

const std::string			server_config::get_root(int s, int l) const
{
	if (l == -1)
		return server[s].root;
	else
		return server[s].location[l].root;
}

const std::string			server_config::get_uri(int s, int l) const
{
	if (l == -1)
		return ("/");
	else
		return server[s].location[l].path;
}

const std::string			server_config::get_index(int s, int l) const
{
	if (l == -1)
		return server[s].index;
	else
		return server[s].location[l].index;
}

bool						server_config::if_autoindex_on(int s, int l) const
{
	if (l == -1)
		return (server[s].autoindex_on());
	else
		return (server[s].location[l].autoindex_on());
}

const std::string			server_config::get_server_name(int s) const
{
	return server[s].server_name[0];
}

const std::pair<std::string, std::string>	server_config::get_error_page(int s, int l) const
{
	if (l == -1)
		return server[s].error_page;
	else
		return server[s].location[l].error_page;
}

void						server_s::complete_locations(void)
{
	std::vector<location_s>::iterator	ite;

	for (ite = this->location.begin(); ite != this->location.end(); ++ite)
	{
		if (ite->root.empty())
			ite->root = this->root + ite->path;
		if (ite->index.empty())
			ite->index = this->index;
		if (ite->autoindex.empty())
			ite->autoindex = this->autoindex;
		if (ite->allowed_methods.empty())
			ite->allowed_methods = this->allowed_methods;
		if (ite->client_max_body_size == -1)
			ite->client_max_body_size = this->client_max_body_size;
		if (ite->error_page.first.empty())
			ite->error_page = this->error_page;
	}
}
