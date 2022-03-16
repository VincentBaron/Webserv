/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 10:27:29 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/16 11:31:28 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include "common.hpp"
#include "Parsing.hpp"
#include "Parsing_request.hpp"
#include <sys/wait.h>
#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX_LINE 1000000
#define SA_IN struct sockaddr_in
#define SA struct sockaddr

class Socket
{

public:
	typedef std::pair<int, int> intPair;
	typedef std::pair<int, std::string> intStrPair;
	typedef std::vector<intStrPair>::iterator intStrIte;
	typedef std::vector<intPair>::iterator iterator;
	typedef std::vector<server_s>::iterator confIte;
	// Constructors and destructor
	Socket(void);
	virtual ~Socket();

	// Member functions

	void initSocket(server_config &conf);

	int accept_new_connection(int server_socket);

	void clearClient(iterator clientIte);

	void handle_connection(iterator clientIte, server_config &conf);

	iterator checkIsServer(int i);

	void handle_response(intStrIte clientIte);

	void waitForConnections(server_config &conf);

private:
	std::vector<intPair> servers;
	std::vector<intPair> clients;
	std::vector<intStrPair> clientsResps;
	server_config confFile;
	fd_set master_read_socks, read_sockets, master_write_socks, write_sockets;
};

#endif
