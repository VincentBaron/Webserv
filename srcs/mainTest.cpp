/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainTest.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/01 13:05:24 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/03 12:19:28 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/common.hpp"
#include "../includes/get_next_line.h"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_LINE 1000

void err_n_die(std::string mssg)
{
	std::cerr << "Error: " << mssg << std::endl;
	exit(EXIT_FAILURE);
}

int main(void)
{
	typedef struct sockaddr SA;

	int listenfd, connfd, n;
	struct sockaddr_in servaddr;
	std::string buff;
	char reqBuffer[MAX_LINE + 1];

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_die("Can't create socket!!!");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if ((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0)
		err_n_die("Can't bind!!!");

	if ((listen(listenfd, 10)) < 0)
		err_n_die("Listen error!!!");
	while (1)
	{
		std::cout << "Waitin for a connection..." << std::endl;
		connfd = accept(listenfd, (SA *)NULL, NULL);
		recv(connfd, reqBuffer, 1000, 0);
		std::cout << "" << reqBuffer << std::endl;
		buff = "HTTP/1.0 200 OK\r\n\r\nHello";
		send(connfd, buff.c_str(), buff.size(), 0);
		close(connfd);
	}
	return (0);
}