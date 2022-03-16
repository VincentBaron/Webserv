/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 12:59:51 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/16 13:14:41 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cgi.hpp"

Cgi::Cgi(void) {}
Cgi::~Cgi(void) {}

void Cgi::init_vars(std::string file_path)
{
    _path = file_path;
    std::string pwd = getcwd(NULL, 0);

//..Get path of CGI executable
    std::string cgi_path = pwd + "/cgi_executable_mac/php-cgi";
    _vars = (char **)malloc(sizeof(char *) * (MAX_ARGS + 1));

//..Get path of file to send to cgi
    char *name = strdup((char *)_path.c_str());
    char *path = strdup((char *)cgi_path.c_str());
    if (name == NULL)
        err_n_die("Error creating name");
    if (path == NULL)
        err_n_die("Error creating path");

//..Group all path variables into vars to send to cgi executable later
    _vars[0] = path;
    _vars[1] = name;
    _vars[2] = NULL;
}

void Cgi::set_vars(client_request request)
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

    if (request.method == "GET" && request.query_string != "")
    {
        _env["QUERY_STRING"] = request.query_string;
        _body = request.query_string;
    }
    if (request.method == "POST")
    {
        std::stringstream content_len;
        content_len << request.body.size();
        _env["CONTENT_LENGTH"] = content_len.str();
        // std::cout << "_env[\"CONTENT_LENGTH\"] " << _env["CONTENT_LENGTH"] << std::endl;
        _env["CONTENT_TYPE"] = request.header_fields.find("Content-Type")->second;
        _body = request.body;
    }

    _envs = (char **)malloc(sizeof(char *) * (_env.size() + 1));
    if (_envs == NULL)
        err_n_die("Error malloc!");

    int i = 0;
    for (std::map<std::string, std::string>::iterator ite = _env.begin(); ite != _env.end(); ite++)
    {
        _envs[i] = strdup((ite->first + "=" + ite->second).c_str());
        if (_envs[i] == NULL)
            err_n_die("Error envs vars");
        i++;
    }
    _envs[i] = NULL;
}

void Cgi::execute_cgi(void)
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

void Cgi::remove_headers(void)
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
        value = header.substr(pos2 + 2, pos - pos2 - 2);
        _cgi_content[key] = value;
        header.erase(0, pos + 2);
    }
}

std::string Cgi::get_response(void) { return (_response); };