/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:10:51 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/16 17:52:47 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

Socket::Socket() {}

Socket::~Socket() {}

void Socket::initSocket(server_config &conf)
{
    SA_IN servaddr;
    int opt = TRUE;

//..Iterate over all servers present in config file  to initiate them  
    server_config confFile = conf;
    for (confIte ite = conf.server.begin(); ite != conf.server.end(); ite++)
    {
        
//......Inititate server socket        
        int server_fd;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            err_n_die("Can't create socket!!!");

//......Add security to avoid binding error if servers are killed and launched in series        
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
            err_n_die("Error setsockopt!!!");

//......Set server port specifications for connection        
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(ite->port[0]);

//......Bind server to port        
        if ((bind(server_fd, (SA *)&servaddr, sizeof(servaddr))) < 0)
            err_n_die("Can't bind!!!");

//......Make server listen to incoming connections        
        if ((listen(server_fd, FD_SETSIZE)) < 0)
            err_n_die("Listen error!!!");
        this->servers.insert(std::make_pair(server_fd, ite->port[0]));
    }
}

int Socket::accept_new_connection(int server_socket)
{
//..Create new client socket once server is ready    
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;
    if ((client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size)) < 0)
        err_n_die("Error accepting new client connection!");
    return (client_socket);
}

void Socket::clearClient(iterator clientIte)
{
//..Clear client from everything if error errupted    
    FD_CLR((*clientIte).first, &master_read_socks);
    FD_CLR((*clientIte).first, &master_write_socks);
    int tmp = ((*clientIte).first);
    clients.erase(clientIte);
    close(tmp);
}

void Socket::handle_connection(Socket::iterator clientIte, server_config &conf)
{
    char reqBuffer[MAX_LINE + 1];
    std::string buff;
    client_request clientReq;
    std::string response;

//..Receive request from new client    
    memset(reqBuffer, 0, MAX_LINE + 1);
    if ((recv((*clientIte).first, reqBuffer, MAX_LINE + 1, 0)) == -1)
    {
        std::cout << "Error with recv!" << std::endl;
        clearClient(clientIte);
        return;
    }
//..Manage incoming request to produce a response from server
    clientReq.set_port((*clientIte).second);
    clientReq.parse_request(reqBuffer);
    response = clientReq.process_request(conf);
//..Add client to list of fd which can be written to
    FD_SET((*clientIte).first, &master_write_socks);
    clientsResps.insert(std::make_pair((*clientIte).first, response));
//..Remove client from reaad fds list
    clients.erase(clientIte);
}

Socket::iterator Socket::checkIsServer(int i)
{

//..Check that a fd is part of servers in config file    
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
//..Send server response to client    
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
//..Terminate connection
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

//..Add all servers to read_fds    
    for (std::map<int, int>::iterator ite = servers.begin(); ite != servers.end(); ite++)
    {
        FD_SET((*ite).first, &master_read_socks);
        if ((*ite).first > max_socket)
            max_socket = (*ite).first;
    }
    std::cout << "Waiting for a connection..." << std::endl;
    while (1)
    {
//......Set tmp read and write fd sets to master fd sets.        
        FD_ZERO(&read_sockets);
        FD_ZERO(&write_sockets);
        read_sockets = master_read_socks;
        write_sockets = master_write_socks;
//......Wait for Select to detect activity on read or write side
        if (select(max_socket + 1, &read_sockets, &write_sockets, NULL, NULL) < 0)
            err_n_die("Select failed!!");
//.....Iterate over all fds
        for (int i = 0; i <= max_socket; i++)
        {
//..........If an fd can be written to, send it the associated server response            
            if (FD_ISSET(i, &write_sockets))
            {
                std::map<int, std::string>::iterator ite = clientsResps.find(i);
                if (ite != clientsResps.end())
                    handle_response(ite);
            }
//..........If information can be read from a socket
            else if (FD_ISSET(i, &read_sockets))
            {

                Socket::iterator serverIte = checkIsServer(i);
//..............If the read socket is a server create a new client
                if (serverIte != servers.end())
                {
                    int client_socket = accept_new_connection(i);
                    clients.insert(std::make_pair(client_socket, (*serverIte).second));
                    FD_SET(client_socket, &master_read_socks);
                    if (client_socket > max_socket)
                        max_socket = client_socket;
                }
//..............If the read socket is a client read what it has to send to server
                else
                {
                    std::map<int, int>::iterator ite = clients.find(i);
                    if (ite != clients.end())
                        handle_connection(ite, conf);
                }
            }
        }
    }
}
