#ifndef PARSING_REQUEST_HPP
# define PARSING_REQUEST_HPP

# include <vector>
# include <string>
# include <iostream>
# include <sys/stat.h>
# include <map>
# include <sstream>
# include "Parsing.hpp"
# include <ctime>
# include <sys/types.h>
# include <dirent.h>

# define MAX_METHOD_SIZE 6

class client_request
{
	public:
		std::string			method;
		std::string			request_target;
		std::string			http_version;
		int					port;
		std::map<std::string, std::string>		header_fields; //important: host
		std::string			body;
		std::string			error;
		std::string			query_string;

		//Variable used for the response
		
		int					reponse_len;

	public:
		client_request() : port(8080), error("200"),  _r_body(false), server_pos(-1), location_pos(-1)
		{
			file_types = initialize_file_types();
			error_reponse = initialize_error_reponse();
		}

		bool		reading_body() const { return (_r_body); }
		void		set_rbody(bool v) { _r_body = v; }
		void		set_port(int p) { port = p; }

		void		parse_request(std::string input);
		std::string	process_request(server_config const server);

	private:

		std::map<std::string, std::string>		initialize_file_types();
		std::map<std::string, std::string>		initialize_error_reponse();
		int										parse_line_request(std::string line);
		int										parse_header_line(std::string line);

		std::string		process_get(server_config const server);
		std::string		process_post(server_config const server);
		std::string		process_delete(server_config const server);
		std::string		process_error(server_config const server);
		std::string		process_cgi(std::string file_path);

		std::string		process_redirection(std::pair<std::string, std::string> const redirection);

	private:
		bool						_r_body;
		std::map<std::string, std::string>	file_types;
		std::map<std::string, std::string>	error_reponse;
		int							server_pos; //server and loc position in sever class vector;
		int							location_pos;
};

#endif
