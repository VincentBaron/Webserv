/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 13:32:44 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/16 15:39:35 by daprovin         ###   ########.fr       */
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
#include <sys/wait.h>
#define MAX_ARGS 2
#define READ 0
#define WRITE 1

class Cgi
{

public:
    // Constructors and destructor
    Cgi(void);
    virtual ~Cgi(void);

    // Operator overloads

    // Getters / Setters

    // Member functions

    void init_vars(std::string file_path);

    void set_vars(client_request request);

    void execute_cgi(void);

    void remove_headers(void);

    std::string get_response(void);


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
	// Private functions
	void free_all(void);
};

#endif
