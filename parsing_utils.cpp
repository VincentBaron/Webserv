#include "Parsing.hpp"
#include <fstream>

std::string		parse_index(std::string	line)
{
	std::string		ret;
	int				pos;

	line.erase(0, 5);
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find_first_of(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}
	ret = line.substr(0, pos);

	return ret;
}

std::vector<int>		parse_port(std::string line)
{
	std::vector<int>		ret;
	std::string				delim = " ";
	std::string				port_num;
	int						pos;

	line.erase(0, 6);									//remove "listen" ";" and first spaces and tabs
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}

	line.erase(pos);

	while ((pos = line.find(delim)) != std::string::npos) //split the line and store the numbers
	{
		port_num = line.substr(0, pos);
		ret.push_back(atoi(port_num.c_str()));
		line.erase(0, pos + 1);
	}
	ret.push_back(atoi(line.c_str()));

	return ret;
}

std::string		parse_root(std::string line)
{
	std::string		ret;
	int				pos;

	line.erase(0, 4);
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find_first_of(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}
	ret = line.substr(0, pos);

	return ret;
}

std::vector<std::string>		parse_name(std::string line)
{
	std::vector<std::string>		ret;
	std::string						delim = " ";
	int								pos;

	line.erase(0, 11);									//remove "listen" ";" and first spaces and tabs
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}

	line.erase(pos);

	while ((pos = line.find(delim)) != std::string::npos) //split the line and store the names
	{
		ret.push_back(line.substr(0, pos));
		line.erase(0, pos + 1);
	}
	ret.push_back(line.c_str());

	return ret;
}

std::pair<std::string, std::string>		parse_error_page(std::string line)
{
	std::pair<std::string, std::string>		ret;
	int										pos;
	std::string								delim = " ";

	line.erase(0, 10);									//remove "listen" ";" and first spaces and tabs
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}

	line.erase(pos);

	pos = line.find(delim);
	ret.first = line.substr(0, pos);
	line.erase(0, pos + 1);
	pos = line.find(delim);
	ret.second = line.substr(0, pos);
	
	return ret;
}

int			parse_body_size(std::string	line)
{
	int				ret;
	int				pos;

	line.erase(0, 20);			
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find(';');

	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}

	line.erase(pos);

	ret = atoi(line.c_str());
	return ret;
}

bool		is_allowed_method(std::string tmp)
{
	if (tmp != "GET" && tmp != "POST" && tmp != "DELETE")
		return (false);
	return (true);
}

std::vector<std::string>	parse_methods(std::string line)
{
	int							pos;
	std::string					delim = " ";
	std::vector<std::string>	ret;
	std::string					tmp;

	line.erase(0, 12);			
	pos = line.find_first_not_of(" \t");
	line.erase(0, pos);
	pos = line.find(';');
	
	if (pos == std::string::npos)              //error if there is not ";"
	{
		std::cout << "Syntax error:" << line << std::endl;
		exit(1);
	}

	line.erase(pos);

	while ((pos = line.find(delim)) != std::string::npos) //split the line and store the methods
	{
		tmp = line.substr(0, pos);
		if (!is_allowed_method(tmp))
		{
			std::cout << "Method not allowed: " << tmp << std::endl;
			exit(1);
		}
		ret.push_back(tmp);
		line.erase(0, pos + 1);
	}
	tmp = line.substr(0, pos);
	if (!is_allowed_method(tmp))
	{
		std::cout << "Method not allowed: " << tmp << std::endl;
		exit(1);
	}
	ret.push_back(tmp);

	return ret;
}
