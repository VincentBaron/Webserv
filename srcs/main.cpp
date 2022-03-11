/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:25:46 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/11 16:23:28 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"
#include "../includes/Parsing.hpp"
# define MAX_QUEU 10



int main(int ac, char **av)
{
	Socket server;
	server_config conf_file;
	
	// 1. Parse configuration file
	parsing(ac, av, conf_file);
	// 2. Create server and init.
	server.initSocket(conf_file);
	server.waitForConnections(conf_file);
	// basicClientAndServer();
}