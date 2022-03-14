/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 13:32:44 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/14 20:55:06 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP
#include <string>
#include <unistd.h>
#include <map>
#include "common.hpp"
#include "Parsing_request.hpp"
#define MAX_ARGS 2

class Cgi
{

public:
    // Constructors and destructor
    Cgi(void);
    Cgi(const Cgi &src);
    virtual ~Cgi();

    // Operator overloads
    Cgi &operator=(const Cgi &rhs);

    // Getters / Setters

    // Member functions
    void get_path(std::string content)
    {

        char *path = NULL;
        _path = "/" + content;
        std::string tmp = _path;
        path = getcwd(NULL, 0);
        _pwd = path;
        if (path == NULL)
            err_n_die("Error with cwd");
        else
        {
            _path = _pwd;
            if (_path.find("/www") != std::string::npos)
                _path += tmp;
            else
                _path += "/www" + tmp;
            free(path);
        }
    };

    void init_vars()
    {
        std::string cgi_path = "/usr/local/bin/php-cgi";
        _vars = (char **)malloc(sizeof(char *) * (MAX_ARGS + 1));
        char *name = strdup((char *)_path.c_str());
        char *path = strdup((char *)cgi_path.c_str());
        if (name == NULL)
            err_n_die("Error creating name");
        if (path == NULL)
            err_n_die("Error creating path");
        *_vars = path;
        _vars++;
        *_vars = name;
        _vars++;
        *_vars = NULL;
    };

    void set_env(client_request request)
    {
        _body = "";
        _env["AUTH_TYPE"] = "";
        _env["CONTENT_LENGTH"] = "0";
        _env["GATEWAY_INTERFACE"] = "CGI/1.1";
        _env["PATH_TRANSLATED"] = ""; //_args[0];
        _env["REDIRECT_STATUS"] = "200";
        _env["REQUEST_METHOD"] = request.method;
        _env["SCRIPT_FILENAME"] = _vars[1];
        _env["SCRIPT_PORT"] = request.port;
        _env["SERVER_NAME"] = "webserv";
        _env["SERVER_PROTOCOL"] = "HTTP/1.1";

        if (request.method == "GET" && request.query != "")
        {
            _env["QUERY_STRING"] = request.query;
            _body = request.query;
        }
        if (method == "POST")
        {
            _env["CONTENT_LENGTH"] = 
        }
    }

private:
    // Attributes
    std::string _path;
    std::string _pwd;
    std::string _body;
    std::map<std::string, std::string> _env;
    char **_vars;
};

#endif