#include "Socket.hpp"
#include <cstring>
#include <fstream>


#define PORT 8080

int	main()
{
	Socket				server;
	Socket				new_socket;
	struct sockaddr_in	address;
	int					addrlen = sizeof(address);	
	long				valread;
	std::string			hello = "Hello from sesrver";

	server.create();

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	server.identify((struct sockaddr *)&address, sizeof(address));
	server.s_listen(10);

	while (1)
	{
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
		new_socket = server.s_accept((struct sockaddr *)&address, (socklen_t *)&addrlen);
		
		char buffer[30000] = {0};
        valread = read(new_socket.get_fd(), buffer, 30000);
		std::cout << buffer << std::endl;
        write(new_socket.get_fd() , hello.c_str() , hello.size());
        printf("------------------Hello message sent-------------------\n");
		new_socket.s_close();
	}

	return (0);
}
