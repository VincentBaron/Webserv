#ifndef PARSING_REQUEST_HPP
# define PARSING_REQUEST_HPP

# include <vector>
# include <string>
# include <iostream>

# define GET 0
# define POST 1
# define DELETE 2

class client_request
{
	public:
		int					method;
		std::string			request_target;
		std::string			http_version;
		std::string			host;
		std::string			user_agent;
		std::vector<std::string>	accept;
		std::string			accept_language;		
		std::string			accept_encoding;
		std::string			connection;
		std::string			referer;

		int					error;
};

#endif
