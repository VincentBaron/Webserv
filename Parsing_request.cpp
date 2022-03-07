#include "Parsing_request.hpp"

int			parse_line_request(std::string line, client_request & request)
{
		int				pos;
		std::string		tmp;

//.......line structure [method] SP [request-target] SP [http-version]

		pos = line.find(' ');
		tmp = line.substr(0, pos);			//tmp = [method]
		if (tmp.size() > MAX_METHOD_SIZE)
		{
			request.error = 501;
			return 1;
		}
		else if (tmp == "GET")
			request.method = GET;
		else if (tmp == "POST")
			request.method = POST;
		else if (tmp == "DELETE")
			request.method = DELETE;
		else
		{
			request.error = 400;
			return 1;
		}
		line.erase(0, pos + 1);

		pos = line.find(' ');			//go to next SP and store [request-target]
		request.request_target = line.substr(0, pos);
		line.erase(0, pos + 1);

		if (line.empty() || line.find(' ') != std::string::npos) //if there is not [http-version] or another SP, error: 400
		{
			request.error = 400;
			return 1;
		}

		request.http_version = line; //store [http-version]

		return 0;
}

int			parse_header_line(std::string line, client_request & request)
{
	int				pos;	
	std::pair<std::string, std::string>		field;

//..........line structure [field-name] ":" OWS [field-value] OWS

	if ((pos = line.find(':')) == std::string::npos)
	{
		request.error = 400;
		return 1;
	}
	field.first = line.substr(0, pos); //store [field-name]
	line.erase(0, pos + 1);
	
	if (line[0] == ' ' || line[0] == '\t')
		line.erase(0, 1);					//erase OWS

	pos = line.size() - 1;
	if (line[pos] == ' ' || line[pos] == '\t')
		line = line.substr(0, pos);
	field.second = line; 

	request.header_fields.insert(field);
	return 0;
}

void		parse_request(std::string input, client_request & request)
{	
	std::string		line;
	std::string		tmp;
	int				pos;
	
	if (request.reading_body())
		request.body += input;
	else
	{
		pos = input.find("\r\n");
		line = input.substr(0, pos);
		if (parse_line_request(line, request))
			return ;
		input.erase(0, pos + 2);

		while ((pos = input.find("\r\n")) != std::string::npos && input[pos + 2] != std::string::npos)
		{
			pos = input.find("\r\n"); line = input.substr(0, pos);	
			if (parse_header_line(line, request))
				return ;
			input.erase(0, pos + 2);
			//...........Postman para ver una post request...........
			/* if (input[0] == '\n') */
			/* { */
			/* 	input.erase(0, 1); */
			/* 	request.body += input; */
			/* 	request.set_rbody(true); */
			/* } */
		}
	}
}
