#include "Parsing_request.hpp"

std::string		client_request::process_request(server_config const data)
{
//......Serching a server matching the port

	std::cout << "data: " << &data << std::endl;
	for (size_t i = 0; i < data.server.size(); i++)
	{
		for (size_t j = 0; j < data.server[i].port.size(); j++)
		{
			std::cout << "config ports: " << data.server[i].port[j] << std::endl;
			std::cout << "vince port: " << port << std::endl;
			if (data.server[i].port[j] == this->port)
				this->server_pos = i;			
		}
	}

	if (this->server_pos == -1) //internal error, something wrong with port;
	{
		this->error = "500";
		std::cout << "ERROR HERE:" << this->error << std::endl;
		return NULL; //----WARNING-----  write return_error() function
	}

//.....Searching if ther is a location matching URL inside the server

	std::string		relative_path;
	std::string		location_path;

	for (size_t i = 0; i < data.server[this->server_pos].location.size(); i++)
	{
		location_path = data.server[this->server_pos].location[i].path;
		relative_path = this->request_target.substr(0, location_path.size());
		if (location_path.size() >= this->request_target.size() || this->request_target[location_path.size()] == '/')
		{
			if (location_path == relative_path)
				this->location_pos = i;
		}
	}

//......Check if the method is allowed

	bool	flag = true;

	for (size_t i = 0; i < data.get_allowed_methods(server_pos, location_pos).size(); i++)
	{
		if (this->method == data.get_allowed_methods(server_pos, location_pos)[i])	
			flag = false;
	}

	if (flag)
	{
		this->error = "500";
		std::cout << "ERROR HERE:" << this->error << std::endl;
		return NULL; //----WARNING-----  write return_error() function
	}

//..........Select fuction to use

	if (this->method == "GET")
		return	this->process_get(data);
	/* else if (this->method == "POST") */
	/* 	return	this->process_post(data); */
	/* else if (this->method == "DELETE") */
	/* 	return	this->process_delete(data); */

	return NULL;
}

std::string		ft_gettime()
{
	time_t		now = time(0);
	char*		dt = ctime(&now);

	tm	*gmtm = gmtime(&now);
	dt = asctime(gmtm);

	std::string		tmp(dt);
	std::string		t_str;

	t_str = tmp.substr(0, 3) + ", " + tmp.substr(8, 2) + " " + tmp.substr(4, 3) + " " + tmp.substr(20, 4) + " " + tmp.substr(11, 8) + " " + "GMT";

	return t_str;
}

bool		path_is_dir(std::string path)
{
	struct stat		s;
	if (lstat(path.c_str(), &s) == 0)
	{
		if (S_ISDIR(s.st_mode))
			return true;
		else
			return false;
	}
	return false;
}

bool		path_is_file(std::string path)
{
	struct stat		s;
	if (lstat(path.c_str(), &s) == 0)
	{
		if (S_ISREG(s.st_mode))
			return true;
		else
			return false;
	}
	return false;
}

std::string	client_request::process_get(server_config const config)
{
	std::string		ret;
	std::ifstream	req_file;	
	std::string		reponse_body;
	std::string		file_path = config.get_root(this->server_pos, this->location_pos) + this->request_target; 

	if (path_is_dir(file_path))
	{
		if (config.if_autoindex_on(this->server_pos, this->location_pos))
		{}
		else
		{
			if (this->request_target == config.get_uri(this->server_pos, this->location_pos))
				file_path += config.get_index(this->server_pos, this->location_pos);
			else
				file_path.clear();	
		}
	}
	/* else if (file_path is a file) */ //I think that i dont need to do anythng if filepath is a file

	//NEED TO DO ADD ERROR REQUESTS FOR FAVICON.
	
	if (!file_path.empty())
	{
		req_file.open(file_path.c_str());
		if (!req_file.is_open())
		{
			this->error = "404";
			std::cout << "ERROR HERE:" << this->error << std::endl;
			return NULL; //----WARNING-----  write return_error() function
		}
		else
		{
			std::ostringstream	ss;
			ss << req_file.rdbuf();
			reponse_body = ss.str();
		}
		req_file.close();
	}
	
	std::string		date = ft_gettime();

	//saving Content Length as an std::string

	std::stringstream content_len;
	content_len << reponse_body.size();

	//searching for content type
	
	std::map<std::string, std::string>::iterator	it;
	std::string										content_type;

	for (it = this->file_types.begin(); it != this->file_types.end(); ++it)
	{
		std::string		extension;
		size_t	pos = file_path.find_last_of(".");
		if (pos != std::string::npos)
			extension = file_path.substr(pos);
		if (it->first == extension)
			content_type = it->second;
	}

	ret = this->http_version + " " + this->error + " " + this->error_reponse.find(this->error)->second + "\r\n";
	ret += "Date: " + date + "\r\n";
	ret += "Server: Webserv\r\n";
	ret += "Conection: Closed\r\n";
	ret += "Content-Length: " + std::string(content_len.str()) + "\r\n";
	ret += "Content-Type: " + content_type + "\r\n";
	ret += "\r\n";
	ret += reponse_body;

	this->reponse_len = ret.size();

	/* std::cout << ret << std::endl; */

	/* const char* c = ret.c_str(); */

	return ret;
}

int			client_request::parse_line_request(std::string line)
{
	size_t			pos;
	std::string		tmp;

	//.......line structure [method] SP [request-target] SP [http-version]

	pos = line.find(' ');
	tmp = line.substr(0, pos);			//tmp = [method]
	if (tmp.size() > MAX_METHOD_SIZE)
	{
		this->error = "501";
		return 1;
	}
	else if (tmp == "GET")
		this->method = "GET";
	else if (tmp == "POST")
		this->method = "POST";
	else if (tmp == "DELETE")
		this->method = "DELETE";
	else
	{
		this->error = "405";
		return 1;
	}
	line.erase(0, pos + 1);

	pos = line.find(' ');			//go to next SP and store [request-target]
	this->request_target = line.substr(0, pos);
	line.erase(0, pos + 1);

	if (line.empty() || line.find(' ') != std::string::npos) //if there is not [http-version] or another SP, error: 400
	{
		this->error = "400";
		return 1;
	}

	this->http_version = line; //store [http-version]

	return 0;
}

int			client_request::parse_header_line(std::string line)
{
	size_t			pos;	
	std::pair<std::string, std::string>		field;

//..........line structure [field-name] ":" OWS [field-value] OWS

	if ((pos = line.find(':')) == std::string::npos)
	{
		this->error = "400";
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

	this->header_fields.insert(field);
	return 0;
}

void		client_request::parse_request(std::string input)
{	
	std::string		line;
	std::string		tmp;
	size_t			pos;
	
	if (this->reading_body())
		this->body += input;
	else
	{
		pos = input.find("\r\n");
		line = input.substr(0, pos);
		if (this->parse_line_request(line))
			return ;
		input.erase(0, pos + 2);

		while ((pos = input.find("\r\n")) != std::string::npos && (pos+ 2) < input.size())
		{
			pos = input.find("\r\n"); line = input.substr(0, pos);	
			if (this->parse_header_line(line))
				return ;
			input.erase(0, pos + 2);
			//...........Postman para ver una post this->..........
			/* if (input[0] == '\n') */
			/* { */
			/* 	input.erase(0, 1); */
			/* 	this->body += input; */
			/* 	this->set_rbody(true); */
			/* } */
		}
	}
}

std::map<std::string, std::string>		client_request::initialize_file_types()
{
    std::map<std::string, std::string> ret;

	ret[".aac"]      = "audio/aac";
	ret[".abw"]      = "application/x-abiword";
	ret[".arc"]      = "application/octet-stream";
	ret[".avi"]      = "video/x-msvideo";
	ret[".azw"]      = "application/vnd.amazon.ebook";
	ret[".bin"]      = "application/octet-stream";
	ret[".bz"]       = "application/x-bzip";
	ret[".bz2"]      = "application/x-bzip2";
	ret[".csh"]      = "application/x-csh";
	ret[".css"]      = "text/css";
	ret[".csv"]      = "text/csv";
	ret[".doc"]      = "application/msword";
	ret[".epub"]     = "application/epub+zip";
	ret[".gif"]      = "image/gif";
	ret[".htm"]      = "text/html";
	ret[".html"]     = "text/html";
	ret[".ico"]      = "image/x-icon";
	ret[".ics"]      = "text/calendar";
	ret[".jar"]      = "Temporary Redirect";
	ret[".jpeg"]     = "image/jpeg";
	ret[".jpg"]      = "image/jpeg";
	ret[".js"]       = "application/js";
	ret[".json"]     = "application/json";
	ret[".mid"]      = "audio/midi";
	ret[".midi"]     = "audio/midi";
	ret[".mpeg"]     = "video/mpeg";
	ret[".mpkg"]     = "application/vnd.apple.installer+xml";
	ret[".odp"]      = "application/vnd.oasis.opendocument.presentation";
	ret[".ods"]      = "application/vnd.oasis.opendocument.spreadsheet";
	ret[".odt"]      = "application/vnd.oasis.opendocument.text";
	ret[".oga"]      = "audio/ogg";
	ret[".ogv"]      = "video/ogg";
	ret[".ogx"]      = "application/ogg";
	ret[".png"]      = "image/png";
	ret[".pdf"]      = "application/pdf";
	ret[".ppt"]      = "application/vnd.ms-powerpoint";
	ret[".rar"]      = "application/x-rar-compressed";
	ret[".rtf"]      = "application/rtf";
	ret[".sh"]       = "application/x-sh";
	ret[".svg"]      = "image/svg+xml";
	ret[".swf"]      = "application/x-shockwave-flash";
	ret[".tar"]      = "application/x-tar";
	ret[".tif"]      = "image/tiff";
	ret[".tiff"]     = "image/tiff";
	ret[".ttf"]      = "application/x-font-ttf";
	ret[".txt"]      = "text/plain";
	ret[".vsd"]      = "application/vnd.visio";
	ret[".wav"]      = "audio/x-wav";
	ret[".weba"]     = "audio/webm";
	ret[".webm"]     = "video/webm";
	ret[".webp"]     = "image/webp";
	ret[".woff"]     = "application/x-font-woff";
	ret[".xhtml"]    = "application/xhtml+xml";
	ret[".xls"]      = "application/vnd.ms-excel";
	ret[".xml"]      = "application/xml";
	ret[".xul"]      = "application/vnd.mozilla.xul+xml";
	ret[".zip"]      = "application/zip";
	ret[".3gp"]      = "video/3gpp audio/3gpp";
	ret[".3g2"]      = "video/3gpp2 audio/3gpp2";
	ret[".7z"]       = "application/x-7z-compressed";

	return ret;
}

std::map<std::string, std::string>		client_request::initialize_error_reponse()
{
    std::map<std::string, std::string> ret;

    ret[ "100" ] = "Continue";
    ret[ "101" ] = "Switching Protocols";
    ret[ "102" ] = "Processing";
    ret[ "103" ] = "Checkpoint";

    ret[ "200" ] = "OK";
    ret[ "201" ] = "Created";
    ret[ "202" ] = "Accepted";
    ret[ "203" ] = "Non-Authoritative Information";
    ret[ "204" ] = "No Content";
    ret[ "205" ] = "Reset Content";
    ret[ "206" ] = "Partial Content";
    ret[ "207" ] = "Multi-Status";
    ret[ "208" ] = "Already Reported";

    ret[ "300" ] = "Multiple Choices";
    ret[ "301" ] = "Moved Permanently";
    ret[ "302" ] = "Found";
    ret[ "303" ] = "See Other";
    ret[ "304" ] = "Not Modified";
    ret[ "305" ] = "Use Proxy";
    ret[ "306" ] = "Switch Proxy";
    ret[ "307" ] = "Temporary Redirect";
    ret[ "308" ] = "Permanent Redirect";

    ret[ "400" ] = "Bad Request";
    ret[ "401" ] = "Unauthorized";
    ret[ "402" ] = "Payment Required";
    ret[ "403" ] = "Forbidden";
    ret[ "404" ] = "Not Found";
    ret[ "405" ] = "Method Not Allowed";
    ret[ "406" ] = "Not Acceptable";
    ret[ "407" ] = "Proxy Authentication Required";
    ret[ "408" ] = "Request Timeout";
    ret[ "409" ] = "Conflict";
    ret[ "410" ] = "Gone";
    ret[ "411" ] = "Length Required";
    ret[ "412" ] = "Precondition Failed";
    ret[ "413" ] = "Payload Too Large";
    ret[ "414" ] = "URI Too Long";
    ret[ "415" ] = "Unsupported Media Type";
    ret[ "416" ] = "Requested Range Not Satisfiable";
    ret[ "417" ] = "Expectation Failed";
    ret[ "418" ] = "I'm a teapot";
    ret[ "421" ] = "Misdirected Request";
    ret[ "422" ] = "Unprocessable Entity";
    ret[ "423" ] = "Locked";
    ret[ "424" ] = "Failed Dependency";
    ret[ "426" ] = "Upgrade Required";
    ret[ "428" ] = "Precondition Required";
    ret[ "429" ] = "Too Many Request";
    ret[ "431" ] = "Request Header Fields Too Large";
    ret[ "451" ] = "Unavailable For Legal Reasons";

    ret[ "500" ] = "Internal Server Error";
    ret[ "501" ] = "Not Implemented";
    ret[ "502" ] = "Bad Gateway";
    ret[ "503" ] = "Service Unavailable";
    ret[ "504" ] = "Gateway Timeout";
    ret[ "505" ] = "HTTP Version Not Supported";
    ret[ "506" ] = "Variant Also Negotiates";
    ret[ "507" ] = "Insufficient Storage";
    ret[ "508" ] = "Loop Detected";
    ret[ "510" ] = "Not Extended";
    ret[ "511" ] = "Network Authentication Required";

    return ret;
}
