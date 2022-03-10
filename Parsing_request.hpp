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

	public:
		client_request() : _r_body(false), error("200"), server_pos(-1), location_pos(-1), port(8080)
		{
			file_types = initialize_file_types();
			error_reponse = initialize_error_reponse();
		}

		const bool	reading_body() const { return (_r_body); }
		void		set_rbody(bool v) { _r_body = v; }
		void		set_port(int p) { port = p; }

		void		parse_request(std::string input);
		char *		process_request(server_config const server);

	private:

		std::map<std::string, std::string>		initialize_file_types();
		std::map<std::string, std::string>		initialize_error_reponse();
		int										parse_line_request(std::string line);
		int										parse_header_line(std::string line);
		


		char *		process_get(server_config const server);
		char *		process_post(server_config const server);
		char *		process_delete(server_config const server);

	private:
		bool						_r_body;
		std::map<std::string, std::string>	file_types;
		std::map<std::string, std::string>	error_reponse;
		int							server_pos; //server and loc position in sever class vector;
		int							location_pos;
};

#endif
