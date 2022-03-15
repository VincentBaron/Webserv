/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 13:32:44 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/15 15:30:51 by daprovin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include "common.hpp"
#include "Parsing_request.hpp"
#define MAX_ARGS 2
#define READ 0
#define WRITE 1

class Cgi
{

public:
    // Constructors and destructor
    Cgi(void){};
    virtual ~Cgi() {};

    // Operator overloads

    // Getters / Setters

    // Member functions

    void init_vars(std::string file_path)
    {
        _path = file_path;
        std::cout << "_path: " << _path << std::endl;
        std::string pwd  = getcwd(NULL, 0);
        std::string cgi_path = pwd + "/cgi_executable/php-cgi";
        _vars = (char **)malloc(sizeof(char *) * (MAX_ARGS + 1));
        char *name = strdup((char *)_path.c_str());
        char *path = strdup((char *)cgi_path.c_str());
        if (name == NULL)
            err_n_die("Error creating name");
        if (path == NULL)
            err_n_die("Error creating path");
        _vars[0] = path;
        _vars[1] = name;
        _vars[2] = NULL;
    };

    void set_vars(client_request request)
    {
        if (request.method == "GET" && request.query_string != "")
        {
            _env["QUERY_STRING"] = request.query_string;
            _body = request.query_string;
        }
        if (request.method == "POST")
        {
            _env["CONTENT_LENGTH"] = request.body.size();
            _env["CONTENT_TYPE"] = request.header_fields.find("Content-Type")->second;
            _body = request.body;
        }
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

        _envs = (char **)malloc(sizeof(char *) * (_env.size() + 1));
        if (_envs == NULL)
            err_n_die("Error malloc!");
        
        int i = 0;
        for (std::map<std::string, std::string >::iterator ite = _env.begin(); ite != _env.end(); ite++)
        {
            _envs[i] = strdup((ite->first + "=" + ite->second).c_str());
            if (_envs[i] == NULL)
                err_n_die("Error envs vars");
            i++;    
        }
        _envs[i] = NULL;
    }

    void execute_cgi()
    {
        int fds[2];
        int status;
        int pid;

        if (pipe(fds) == -1)
            err_n_die("Pipe error!");

        if ((pid = fork()) == -1)
            err_n_die("Fork error!");
        if (pid == 0)
        {
            close(fds[READ]);
            dup2(fds[WRITE], 1);

            if (_body.length() > 0)
            {
                int mini_pipe[2];
                if (pipe(mini_pipe) == -1)
                    err_n_die("Pipe error!");
                dup2(mini_pipe[READ], 0);
                write(mini_pipe[WRITE], _body.c_str(), _body.size());
                close(mini_pipe[WRITE]);
            }

            if (execve(_vars[0], _vars, _envs) == -1)
                err_n_die("execve error!");
            exit(0);
        }
        else
        {
            close(fds[WRITE]);
            int ret = wait(&status);
            if (ret == -1)
                err_n_die("wait error");
            if (status == -1)
                err_n_die("child process error");
            char c;
            while (read(fds[READ], &c, 1) > 0)
                _response += c;
        }
        // std::cout << "response: " << _response << std::endl;
    }

    void remove_headers(void)
    {
        std::string header("");
        std::string key;
        std::string value;

        size_t pos2;
        size_t pos = _response.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            header = _response.substr(0, pos + 2);
            _response = _response.substr(pos + 4, std::string::npos);
        }
        while ((pos = header.find("\r\n")) != std::string::npos && (pos2 = header.find(":")) != std::string::npos && pos2 < pos)
        {
            key = header.substr(0, pos2);
            value = header.substr(pos2 + 2, pos - pos2 -2);
            _cgi_content[key] = value;
            header.erase(0, pos + 2);
        }
    }

    std::string get_response(void) {return (_response);};

private:
    // Attributes
    std::string _path;
    std::string _response;
    std::string _pwd;
    std::string _body;
    std::map<std::string, std::string> _env;
    std::map<std::string, std::string> _cgi_content;
    char **_vars;
    char **_envs;
};

#endif
