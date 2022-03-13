#ifndef PARSING_HPP
# define PARSING_HPP

# include <vector>
# include <iostream>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fstream>

class location_s // must construct with the values from the server
{
	public:
		std::string							path;
		std::string							root;						
		std::string							index;
		std::string							autoindex;
		std::vector<std::string>			allowed_methods;
		std::pair<std::string, std::string>	error_page;
		int									client_max_body_size;
	
	public:
		location_s() : client_max_body_size(-1) {
			/* allowed_methods.push_back("GET"); */
			/* allowed_methods.push_back("POST"); */
			/* allowed_methods.push_back("DELETE"); */
		}
		
		bool	if_max_body_size() const { return (client_max_body_size != -1); }
		bool	if_root() const { return (root.size() != 0); }
		bool	autoindex_on() const{ return (autoindex == "on"); }
};

class server_s
{
	public:
		std::vector<std::string>				server_name;
		std::string								root;
		std::string								index;
		std::string								autoindex; //add parsing
		std::pair<std::string, std::string>		error_page;
		std::vector<int>						port;
		std::vector<location_s>					location;
		std::vector<std::string>				allowed_methods;
		int										client_max_body_size;

	public:
		server_s() : autoindex("off"), client_max_body_size(100000) {
			allowed_methods.push_back("GET");
			allowed_methods.push_back("POST");
			allowed_methods.push_back("DELETE");
		}

		int		number_of_ports() const	{ return port.size(); }
		int		number_of_names() const { return server_name.size(); }
		int		number_of_locations() const { return location.size(); }
		bool	if_error_page() const { return (error_page.first.size() != 0); }
		bool	autoindex_on() const { return (autoindex == "on"); }
		void	complete_locations(void);
};

class server_config
{
	public:
		std::vector<server_s>	server;

	public:
		bool								if_autoindex_on(int s, int l) const;
		const std::vector<std::string>		get_allowed_methods(int s, int l) const;
		const std::string					get_root(int s, int l) const;
		const std::string					get_uri(int s, int l) const;
		const std::string					get_index(int s, int l) const;
		const std::string					get_server_name(int s) const;
		int									get_server_max_body_size(int s, int l) const;
		const std::pair<std::string, std::string>		get_error_page(int s, int l) const;
};

void									parsing(int ac, char **av, server_config & data);
std::vector<int>						parse_port(std::string line);
std::string								parse_root(std::string line);
std::vector<std::string>				parse_name(std::string line);
std::pair<std::string, std::string>		parse_error_page(std::string line);
int										parse_body_size(std::string	line);
std::vector<std::string>				parse_methods(std::string line);
std::string								parse_index(std::string	line);
std::string								parse_autoindex(std::string line);

#endif
