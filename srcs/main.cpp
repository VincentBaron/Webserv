/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:25:46 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/07 10:26:57 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"
# define MAX_QUEU 10



int main(void)
{
	Socket server;
	
	// 1. Parse configuration file

	// 2. Create server and init.
	server.initSocket();
	server.waitForConnections();
	// basicClientAndServer();
}