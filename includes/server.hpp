/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:27:29 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/11 11:44:34 by vincentbaro      ###   ########.fr       */
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

	typedef std::pair<int, int> intPair;
	typedef std::vector<intPair>::iterator iterator;
	typedef std::vector<server_s>::iterator confIte;
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

		for (confIte ite = conf.server.begin(); ite != conf.server.end(); ite++)
		{
			int server_fd;
			if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				err_n_die("Can't create socket!!!");

			bzero(&servaddr, sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			servaddr.sin_port = htons(ite->port[0]);

			if ((bind(server_fd, (SA *)&servaddr, sizeof(servaddr))) < 0)
				err_n_die("Can't bind!!!");

			if ((listen(server_fd, 10)) < 0)
				err_n_die("Listen error!!!");
			servers.push_back(std::make_pair(server_fd, ite->port[0]));
		}
		// for (std::vector<int>::iterator ite = servers.begin(); ite != servers.end(); ite++)
		// 	std::cout << "servers" << *ite << std::endl;
	}

	int accept_new_connection(int server_socket)
	{
		int addr_size = sizeof(SA_IN);
		int client_socket;
		SA_IN client_addr;
		if ((client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size)) < 0)
			err_n_die("Error accepting new client connection!");
		return (client_socket);
	}

	void handle_connection(iterator clientIte)
	{
		char reqBuffer[MAX_LINE + 1];
		std::string buff;

		memset(reqBuffer, 0, strlen(reqBuffer));
		recv((*clientIte).first, reqBuffer, 1000, 0);
		std::cout << "" << reqBuffer << std::endl;
		// Parse_request(char * buffer) => while loop (until max-length || strlen(reqBuffer))
		// manage_request();
		buff = "HTTP/1.0 200 OK\r\n\r\nHello";
		send((*clientIte).first, buff.c_str(), buff.size(), 0);
		close((*clientIte).first);
	}

	iterator checkIsServer(int i)
	{
		
		iterator ite;
		for (ite = servers.begin(); ite != servers.end(); ite++)
		{
			if ((*ite).first == i)
				break ;
		}
		return ite;
	}

	void waitForConnections(void)
	{
		fd_set current_sockets, ready_sockets;
		FD_ZERO(&current_sockets);
		FD_ZERO(&ready_sockets);
		int max_socket = -1;
		for (iterator ite = servers.begin(); ite != servers.end(); ite++)
		{
			FD_SET((*ite).first, &current_sockets);
			if ((*ite).first > max_socket)
				max_socket = (*ite).first;
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
					iterator serverIte = checkIsServer(i);
					if (serverIte != servers.end())
					{
						int client_socket = accept_new_connection(i);
						clients.push_back(std::make_pair(client_socket, (*serverIte).second));
						FD_SET(client_socket, &current_sockets);
						if (client_socket > max_socket)
							max_socket = client_socket;
					}
					else
					{
						iterator ite;
						for (ite = clients.begin(); ite != clients.end(); ite++)
						{
							if ((*ite).first == i)
								break ;
						}
						handle_connection(ite);
						clients.erase(ite);
						FD_CLR(i, &current_sockets);
					}
				}
			}
		}
	}

private:
	std::vector<intPair> servers;
	std::vector<intPair> clients;
	int max_clients;
};

#endif