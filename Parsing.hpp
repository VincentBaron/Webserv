#ifndef PARSING_HPP
# define PARSING_HPP

# include <vector>
# include <iostream>
# include <string>

class location_s
{
	public:
		std::vector<location_s> location;

};

class server_s
{
	public:
		std::string		server_name;
		std::vector<int>		port;
		std::vector<location_s>	location;
};

class server_config
{
	public:
		std::vector<server_s>	server;
};

void	parsing(std::string name, server_config & data);

#endif
