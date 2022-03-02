/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:27:29 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/02 12:45:27 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#define TRUE 1
#define PORT 8888

class Socket
{

public:
	// Constructors and destructor
	Socket(void);
	Socket(const Socket &src);
	virtual ~Socket();

	// Operator overloads
	Socket &operator=(const Socket &rhs);

	// Getters / Setters

	// Member functions

	void initSocket(void)
	{
		// Create socket fd for Socket
		if (!(socket_id = socket(AF_INET, SOCK_STREAM, 0)))
		{
			std::cerr << "Error creating Socket socket!" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Set master socket to allow multiple connections
		if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)TRUE, sizeof(TRUE)) < 0)
		{
			std::cerr << "Error allowing multiple connection on Socket socket!" << std::endl;
			exit(EXIT_FAILURE);
		}
		address.sin_family = AF_INET; // AF_INET is the set of adresses for the internet protocol.
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(PORT);

		if (bind(socket_id, (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			std::cerr << "Error binding socket !" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (listen(socket_id, 3) < 0)
		{
			std::cerr << "Listening failed!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void waitForConnections(void)
	{
		int max_sd;
		int sd;

		for (int i = 0; i < max_clients; i++)
			client_socket[i] = 0;
		while (1)
		{
			// clear the socket set
			FD_ZERO(&readfds);

			// Add master socket to set
			FD_SET(socket_id, &readfds);
			max_sd = socket_id;

			// add child sockets to set
			for (int i = 0; i < max_clients; i++)
			{
				// socket descriptor
				sd = client_socket[i];

				// if valid socket descriptor then add to read list
				if (sd > 0)
					FD_SET(sd, &readfds);

				if (sd > max_sd)
					max_sd = sd;
			}
		}
	}

private:
	// Attributes
	int socket_id;
	struct sockaddr_in address;
	fd_set readfds;
	int max_clients = 30;
	int client_socket[30];
};

#endif