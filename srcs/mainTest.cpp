/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainTest.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/01 13:05:24 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/02 17:18:06 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE   1 
#define FALSE  0 
#define PORT 8888 

int main(void)
{
	
	int opt = TRUE;
	int master_socket , addrlen, new_socket, client_socket[30], max_clients = 30, activity, valread, sd;
	int max_sd;
	struct sockaddr_in address;

	char buffer[1025];

	fd_set readfds;
	const char * message = "ECHO Hello world!";

	// Initialise all clients_socket to 0
	for (int i = 0; i < max_clients; i++)
		client_socket[i] = 0;

	// Create master socket
	if (!(master_socket = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("socket failed!");
		exit(EXIT_FAILURE);
	}

	// set master socket to allow multiple connections
	// just a good habit
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt error!");
		exit(EXIT_FAILURE);
	}
	// type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// bind socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "Listener on port " << PORT  << std::endl;

	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	std::cout << "Waiting for connection ..." << std::endl;
	
	while (1)
	{
		// clear the socket set
		FD_ZERO(&readfds);

		// add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		// add child sockets to set
		for (int i = 0; i < max_clients; i++)
		{
			//socket descriptor
			sd = client_socket[i];

			//if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);
			
			if (sd > max_sd)
				max_sd = sd;
		}

		// Wait for an activity on one of the sockets, timeout is NULL so wait undefinetly
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno !=EINTR))
			std::cout << "Select error" << std::endl;
		// If something happened on the master socket, then its an incoming connection

		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen) < 0))
			{
				perror("accept error");
				exit(EXIT_FAILURE);
			}

			// Inform user of socket number - used in send and receive commands
			std::cout << "New connection, socket fd is, " << new_socket << ", ip is: " << inet_ntoa(address.sin_addr) << ", port is: " << ntohs(address.sin_port) << std::endl;

			// send new connection greeting message
			if( (size_t)send(new_socket, message, strlen(message), 0) != strlen(message) )
			{
				perror("send");
			}

			std::cout << "Welcome message sent successfully" << std::endl;

			//add new socket to aray of sockets
			for (int i = 0; i < max_clients; i++)
			{
				// if position is empty
				if (!client_socket[i])
				{
					client_socket[i] = new_socket;
					std::cout << "Adding to list of sockets as " << i << std::endl;

					break ;
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
				//somebody disconnected, get his details and print
				getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
				std::cout << "Host disconnected, ip " << inet_ntoa(address.sin_addr) << ", port " << ntohs(address.sin_port) << std::endl;

				// CLose the socket and mark as 0 in list of reuse
				close(sd);
				client_socket[i] = 0;
			}

			// echo back the message that came in
			else
			{
				//set the string terminating NULL byte on the end of the data read
				buffer[valread] = '\0';
				send (sd, buffer, strlen(buffer), 0);
			}
			
		}
	}

	return (0);
}