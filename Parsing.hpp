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
		std::string				server_name;
		std::string				root;
		std::vector<int>		port;
		std::vector<location_s>	location;

	public:
		int		number_of_ports() const	{ return port.size(); }
};

class server_config
{
	public:
		std::vector<server_s>	server;
	
	public:
		int		number_of_servers() const { return server.size(); }
};

void	parsing(std::string name, server_config & data);

#endif
