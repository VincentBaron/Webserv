#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h>
# include <iostream>
# include <netinet/in.h>
# include <unistd.h>

class Socket
{
	public:
		Socket() {}
		Socket(const int n) : fd(n) {}
		~Socket() {}

		Socket &	operator=(const Socket & src)
		{
			this->fd = src.get_fd();

			return *this;
		}

		void	create()
		{
			fd = socket(AF_INET, SOCK_STREAM, 0); //need case for fd < 0
		}

		void	identify(const struct sockaddr *address, socklen_t address_len)
		{
			bind(fd, address, address_len); //need case for fd < 0
		}

		void	s_listen(int	backlog)
		{
			listen(fd, backlog);
		}

		Socket		s_accept(struct sockaddr *address, socklen_t *address_len)
		{
			Socket		new_s(accept(fd, address, address_len));

			return new_s;
		}

		void	s_close()
		{
			close(fd);	
		}

		const int		get_fd() const { return fd; }

	private:
		int		fd;
};

#endif
