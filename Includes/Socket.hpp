#ifndef SOCKET_HPP

#define SOCKET_HPP

#include "Request.hpp"
#include "Response.hpp"
#define MAX_EVENTS 1000


class server
{
	public:
		server();
		~server();
		server(server const &obj);
		server& operator=(server const &obj);
		
		void create_serveret();
		void bind_serveret(int port);
		int listen_to_connections();
};

class sock
{
	public :
		sock();
		~sock();
		void prepare_response();
		void sending();
		void reading(int kq, struct kevent *change);
		int sock_fd;
		char reading_buffer[4096];
		struct sockaddr_in host_addr;
		int host_addrlen;
		request req;
		response resp;
		int id;
		int filter;
};


#endif