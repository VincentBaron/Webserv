/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:10:51 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/16 15:46:12 by daprovin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

Socket::Socket() {}

Socket::~Socket() {}

void Socket::initSocket(server_config &conf)
{
    {
        SA_IN servaddr;
        int opt = TRUE;

        server_config confFile = conf;
        for (confIte ite = conf.server.begin(); ite != conf.server.end(); ite++)
        {
            int server_fd;
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                err_n_die("Can't create socket!!!");

            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
                err_n_die("Error setsockopt!!!");

            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(ite->port[0]);

            if ((bind(server_fd, (SA *)&servaddr, sizeof(servaddr))) < 0)
                err_n_die("Can't bind!!!");

            if ((listen(server_fd, FD_SETSIZE)) < 0)
                err_n_die("Listen error!!!");
            this->servers.insert(std::make_pair(server_fd, ite->port[0]));
        }
        // for (std::vector<int>::iterator ite = servers.begin(); ite != servers.end(); ite++)
        // 	std::cout << "servers" << *ite << std::endl;
    }
}

int Socket::accept_new_connection(int server_socket)
{
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;
    if ((client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size)) < 0)
        err_n_die("Error accepting new client connection!");
    return (client_socket);
}

void Socket::clearClient(iterator clientIte)
{
    FD_CLR((*clientIte).first, &master_read_socks);
    FD_CLR((*clientIte).first, &master_write_socks);
    clients.erase(clientIte);
    // close((*clientIte).first);
}

void Socket::handle_connection(Socket::iterator clientIte, server_config &conf)
{
    char reqBuffer[MAX_LINE + 1];
    std::string buff;
    client_request clientReq;
    std::string response;

    memset(reqBuffer, 0, MAX_LINE + 1);
    if ((recv((*clientIte).first, reqBuffer, MAX_LINE + 1, 0)) == -1)
    {
        std::cout << "Error with recv!" << std::endl;
        clearClient(clientIte);
        return;
    }
    clientReq.set_port((*clientIte).second);
    clientReq.parse_request(reqBuffer);
    response = clientReq.process_request(conf);
    FD_SET((*clientIte).first, &master_write_socks);
    clientsResps.insert(std::make_pair((*clientIte).first, response));
    clients.erase(clientIte);
}

Socket::iterator Socket::checkIsServer(int i)
{

    Socket::iterator ite;
    for (ite = servers.begin(); ite != servers.end(); ite++)
    {
        if ((*ite).first == i)
            break;
    }
    return ite;
}

void Socket::handle_response(Socket::intStrIte clientIte)
{
    int ret = send((*clientIte).first, (*clientIte).second.c_str(), (*clientIte).second.size(), 0);
    if (ret == -1)
    {
        std::cout << "Error with send!" << std::endl;
        FD_CLR((*clientIte).first, &master_write_socks);
        clientsResps.erase(clientIte);
        close((*clientIte).first);
        return;
    }
    if (ret == 0)
    {
        return;
    }
    FD_CLR((*clientIte).first, &master_write_socks);
    FD_CLR((*clientIte).first, &master_read_socks);
    int tmp = ((*clientIte).first);
    clientsResps.erase(clientIte);
    close(tmp);
}

void Socket::waitForConnections(server_config &conf)
{

    FD_ZERO(&master_read_socks);
    FD_ZERO(&master_write_socks);
    FD_ZERO(&read_sockets);
    FD_ZERO(&write_sockets);
    int max_socket = -1;
    /* struct timeval time = {2, 0}; */

    for (std::map<int, int>::iterator ite = servers.begin(); ite != servers.end(); ite++)
    {
        FD_SET((*ite).first, &master_read_socks);
        if ((*ite).first > max_socket)
            max_socket = (*ite).first;
    }
    int iteration = 0;
    while (1)
    {
        FD_ZERO(&read_sockets);
        FD_ZERO(&write_sockets);
        read_sockets = master_read_socks;
        write_sockets = master_write_socks;
        std::cout << "Waiting for a connection..." << std::endl;
        if (select(max_socket + 1, &read_sockets, &write_sockets, NULL, NULL) < 0)
            err_n_die("Select failed!!");
        for (int i = 0; i <= max_socket; i++)
        {
            if (FD_ISSET(i, &write_sockets))
            {
                std::cout << "Clients receives response At iteration: " << iteration << std::endl;

                std::map<int, std::string>::iterator ite = clientsResps.find(i);
                if (ite != clientsResps.end())
                    handle_response(ite);
            }

            else if (FD_ISSET(i, &read_sockets))
            {

                Socket::iterator serverIte = checkIsServer(i);
                if (serverIte != servers.end())
                {
                    std::cout << "Server new connection At iteration: " << iteration << std::endl;
                    int client_socket = accept_new_connection(i);
                    clients.insert(std::make_pair(client_socket, (*serverIte).second));
                    FD_SET(client_socket, &master_read_socks);
                    if (client_socket > max_socket)
                        max_socket = client_socket;
                }
                else
                {
                    std::cout << "Clients sends request At iteration: " << iteration << std::endl;

                    std::map<int, int>::iterator ite = clients.find(i);
                    if (ite != clients.end())
                        handle_connection(ite, conf);
                }
            }
        }
        iteration++;
    }
}
