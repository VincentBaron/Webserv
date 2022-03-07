#ifndef PARSING_REQUEST_HPP
# define PARSING_REQUEST_HPP

# include <vector>
# include <string>
# include <iostream>
# include <map>

# define GET 0
# define POST 1
# define DELETE 2
# define MAX_METHOD_SIZE 6

class client_request
{
	public:
		int					method;
		std::string			request_target;
		std::string			http_version;
		std::map<std::string, std::string>		header_fields; //important: host

		std::string			body;

		int					error;

	public:
		client_request() : _r_body(false), error(0) {}

		const bool	reading_body() const { return (_r_body); }

		void	set_rbody(bool v) { _r_body = v; }

	private:
		bool				_r_body;
};

void		parse_request(std::string input, client_request & request);

#endif
