/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:27:29 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/10 18:39:46 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include "common.hpp"
#include "Parsing.hpp"
#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_LINE 1000
#define SA_IN struct sockaddr_in
#define SA struct sockaddr

class Socket
{

public:
	// Constructors and destructor
	Socket(void) : max_clients(30){};
	virtual ~Socket(){};

	// Operator overloads
	Socket &operator=(const Socket &rhs);

	// Getters / Setters

	// Member functions

	void initSocket(server_config conf)
	{
		SA_IN servaddr;

		for (std::vector<server_s>::iterator ite = conf.server.begin(); ite != conf.server.end(); ite++)
		{
			int server_fd;
			if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				err_n_die("Can't create socket!!!");

			bzero(&servaddr, sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			std::cout << "port in conf struct: " << ite->port[0] << std::endl;
			servaddr.sin_port = htons(ite->port[0]);

			if ((bind(server_fd, (SA *)&servaddr, sizeof(servaddr))) < 0)
				err_n_die("Can't bind!!!");

			if ((listen(server_fd, 10)) < 0)
				err_n_die("Listen error!!!");
			server_fds.push_back(server_fd);
		}
	}

	int accept_new_connecton(int server_socket)
	{
		int addr_size = sizeof(SA_IN);
		int client_socket;
		SA_IN client_addr;
		if ((client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size)) < 0)
			err_n_die("Error accepting new client connection!");
		return (client_socket);
	}

	void handle_connection(int client_socket)
	{
		char reqBuffer[MAX_LINE + 1];
		std::string buff;

		memset(reqBuffer, 0, strlen(reqBuffer));
		recv(client_socket, reqBuffer, 1000, 0);
		std::cout << "" << reqBuffer << std::endl;
		// Parse_request(char * buffer) => while loop (until max-length || strlen(reqBuffer))
		// manage_request();
		buff = "HTTP/1.0 200 OK\r\n\r\nHello";
		send(client_socket, buff.c_str(), buff.size(), 0);
		close(client_socket);
	}

	bool checkIsServer(int i)
	{
		for (std::vector<int>::iterator ite = server_fds.begin(); ite != server_fds.begin(); ite++)
		{
			if (*ite == i)
				return true;
		}
		return false;
	}

	void waitForConnections(void)
	{
		fd_set current_sockets, ready_sockets;
		FD_ZERO(&current_sockets);
		int max_socket = -1;
		for (std::vector<int>::iterator ite = server_fds.begin(); ite != server_fds.end(); ite++)
		{
			FD_SET(*ite, &current_sockets);
			if (*ite > max_socket)
				max_socket = *ite;
		}
		
		while (1)
		{
			// FD_ZERO(&ready_sockets);
			ready_sockets = current_sockets;

			std::cout << "Waiting for a connection..." << std::endl;
			if (select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) < 0)
				err_n_die("Select failed!!");
			for (int i = 0; i <= max_socket; i++)
			{
				if (FD_ISSET(i, &ready_sockets))
				{
					if (checkIsServer(i))
					{
						int client_socket = accept_new_connecton(i);
						FD_SET(client_socket, &current_sockets);
						if (client_socket > max_socket)
							max_socket = client_socket;
					}
					else
					{
						handle_connection(i);
						FD_CLR(i, &current_sockets);
					}
				}
			}
		}
	}

private:
	std::vector<int> server_fds;
	std::vector<int> ports;
	int max_clients;
};

#endif