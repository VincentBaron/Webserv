#ifndef PARSING_HPP
# define PARSING_HPP

# include <vector>
# include <iostream>
# include <string>

class location_s
{
	public:
		std::string			path;
		std::string			root;						
		int					client_max_body_size;
	
	public:
		location_s() : path(), root(), client_max_body_size(-1) {}
		
		bool	if_max_body_size() const { return (client_max_body_size != -1); }
		bool	if_root() const { return (root.size() != 0); }
};

class server_s
{
	public:
		std::vector<std::string>				server_name;
		std::string								root;
		std::pair<std::string, std::string>		error_page;
		std::vector<int>						port;
		std::vector<location_s>					location;

	public:
		server_s() : root(), error_page(), port(), location(), server_name() {}

		int		number_of_ports() const	{ return port.size(); }
		int		number_of_names() const { return server_name.size(); }
		int		number_of_locations() const { return location.size(); }
		bool	if_root() const { return (root.size() != 0); }
		bool	if_error_page() const { return (error_page.first.size() != 0); }
};

class server_config
{
	public:
		std::vector<server_s>	server;
	
	public:
		int		number_of_servers() const { return server.size(); }
};

void	parsing(int ac, char **av, server_config & data);
std::vector<int>		parse_port(std::string line);
std::string		parse_root(std::string line);
std::vector<std::string>		parse_name(std::string line);
std::pair<std::string, std::string>		parse_error_page(std::string line);
int			parse_body_size(std::string	line);

#endif
