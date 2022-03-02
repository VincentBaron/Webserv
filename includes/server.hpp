/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:27:29 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/02 17:20:57 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include "common.hpp"

class Socket
{

public:
	// Constructors and destructor
	Socket(void) : max_clients(30){};
	virtual ~Socket() {};

	// Operator overloads
	Socket &operator=(const Socket &rhs);

	// Getters / Setters

	// Member functions

	void initSocket(void)
	{
		int opt = TRUE;

		// Create socket fd for Socket
		if (!(socket_id = socket(AF_INET, SOCK_STREAM, 0)))
		{
			std::cerr << "Error creating Socket socket!" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Set master socket to allow multiple connections
		if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
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
		int max_sd, sd, activity, new_socket, addrlen, valread;
		const char *message = "Hello world!";
		char buffer[1025];

		// Initialise all clients_sockets to 0;
		for (int i = 0; i < max_clients; i++)
			client_socket[i] = 0;

		// accept the incoming connection
		addrlen = sizeof(address);
		std::cout << "Waiting for connection ..." << std::endl;
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

			// wait for an activity on one of the sockets
			activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

			if ((activity < 0) && (errno != EINTR))
				std::cerr << "Select error" << std::endl;

			// If something happened on master socket, then its an incoming connection
			if (FD_ISSET(socket_id, &readfds))
			{
				if ((new_socket = accept(socket_id, (struct sockaddr *)&address, (socklen_t *)&addrlen) < 0))
				{
					std::cerr << "Accept error" << std::endl;
					exit(EXIT_FAILURE);
				}

				// Inform user of socket number - used in send and receive commands
				std::cout << "New connection, socket fd is, " << new_socket << ", ip is: " << inet_ntoa(address.sin_addr) << ", port is: " << ntohs(address.sin_port) << std::endl;

				// send new connection greeting message
				if ((size_t)send(new_socket, message, strlen(message), 0) != strlen(message))
					std::cerr << "Send failure!" << std::endl;
				std::cout << "Welcome message sent successfully!" << std::endl;

				// add new socket to aray of sockets
				for (int i = 0; i < max_clients; i++)
				{
					// if position is empty
					if (!client_socket[i])
					{
						client_socket[i] = new_socket;
						std::cout << "Adding to list of sockets as " << i << std::endl;

						break;
					}
				}
			}
		}

		// else its some IO operation on some other socket
		for (int i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				// Check if it was for closing, and also read the income message
				if (!(valread = read(sd, buffer, 1024)))
				{
					// somebody disconnected, get his details and print
					getpeername(sd, (struct sockaddr *)&address,
								(socklen_t *)&addrlen);
					std::cout << "Host disconnected, ip " << inet_ntoa(address.sin_addr) << ", port " << ntohs(address.sin_port) << std::endl;

					// CLose the socket and mark as 0 in list of reuse
					close(sd);
					client_socket[i] = 0;
				}

				// echo back the message that came in
				else
				{
					// set the string terminating NULL byte on the end of the data read
					buffer[valread] = '\0';
					send(sd, buffer, strlen(buffer), 0);
				}
			}
		}
	}

private:
	// Attributes
	int socket_id;
	struct sockaddr_in address;
	fd_set readfds;
	int max_clients;
	int client_socket[30];
};

#endif