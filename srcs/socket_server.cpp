#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include "Parsing_request.hpp"

#define PORT 8080

client_request	test(std::string input)
{
	/* int		pos; */
	/* std::string		line; */
	/* std::string		tmp; */

	/* pos = input.find("\r\n"); */
	/* line = input.substr(0, pos); */
	/* std::cout << line << std::endl << std::endl; */
	/* input.erase(0, pos + 2); */
	
	/* while ((pos = input.find("\r\n")) != std::string::npos) */
	/* 	{ */
	/* 		line = input.substr(0, pos); */	
	/* 		std::cout << line << std::endl << std::endl; */
	/* 		input.erase(0, pos + 2); */
	/* 		/1* if (input[0] == '\n') *1/ */
	/* 		/1* { *1/ */
	/* 		/1* 	input.erase(1); *1/ */
	/* 		/1* 	request.body += input; *1/ */
	/* 		/1* 	requset._r_body = true; *1/ */
	/* 		/1* } *1/ */
	/* 	} */
	client_request	req;

	req.parse_request(input);

	std::cout << req.method << " " << req.request_target << " " << req.http_version << std::endl;

	std::map<std::string, std::string>::iterator	first = req.header_fields.begin();
	std::map<std::string, std::string>::iterator	last = req.header_fields.end();

	for (; first != last ; ++first)
	{
		std::cout << first->first << ": " << first->second << std::endl;
	}
	if (req.body.size() > 0)
	{
		std::cout << std::endl << req.body << std::endl;
	}

	return req;
}

int main(int argc, char **argv)
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);


	client_request	req;
	server_config	server_conf;
	parsing(argc, argv, server_conf);
    
    /* char hello[] = "Hello from server"; */
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
		std::cout << ".............RAW.............." << std::endl << std::endl;
        printf("%s\n",buffer);
		std::cout << ".............PARSED.............." << std::endl << std::endl;
		req = test(buffer);
		std::string reponse = req.process_request(server_conf);
		/* const char * reponse = req.process_request(server_conf); */
		std::cout << ".............REPONSE.............." << std::endl << std::endl;
        write(1 , reponse.c_str() , req.reponse_len);
        write(new_socket , reponse.c_str() , req.reponse_len);
        /* write(new_socket , hello , 17); */
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }
    return 0;
}
