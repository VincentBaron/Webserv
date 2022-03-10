/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainTest.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/01 13:05:24 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/07 10:21:19 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/common.hpp"
#include "../includes/get_next_line.h"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_LINE 1000
#define SA_IN struct sockaddr_in
#define SA struct sockaddr

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
	buff = "HTTP/1.0 200 OK\r\n\r\nHello";
	send(client_socket, buff.c_str(), buff.size(), 0);
	close(client_socket);
}

int main(void)
{

	int server_fd, connfd, n;
	SA_IN servaddr;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_die("Can't create socket!!!");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if ((bind(server_fd, (SA *)&servaddr, sizeof(servaddr))) < 0)
		err_n_die("Can't bind!!!");

	if ((listen(server_fd, 10)) < 0)
		err_n_die("Listen error!!!");

	fd_set current_sockets, ready_sockets;
	FD_ZERO(&current_sockets);
	FD_ZERO(&ready_sockets);
	FD_SET(server_fd, &current_sockets);
	// int max_socket = server_fd;
	while (1)
	{
		ready_sockets = current_sockets;

		std::cout << "Waiting for a connection..." << std::endl;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
			err_n_die("Select failed!!");
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == server_fd)
				{
					std::cout << "" << "yalaa" << std::endl;
					int client_socket = accept_new_connecton(i);
					FD_SET(client_socket, &current_sockets);
					// if (client_socket > max_socket)
					// 	max_socket = client_socket;
				}
				else
				{
					std::cout << "" << "yalaaa2" << std::endl;
					handle_connection(i);
					FD_CLR(i, &current_sockets);
				}
			}
		}
	}
	return (0);
}