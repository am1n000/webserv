#ifndef SOCKET_HPP

#define SOCKET_HPP

#include "Request.hpp"
#include "Response.hpp"
#define MAX_EVENTS 1000

class sock
{
	public :
		sock();
		sock(int sockfd, int filtr, bool listen_sock);
		~sock();
		void prepare_response(int method);
		bool sending(int kq);
		void reading(int kq);
		int sock_fd;
		struct sockaddr_in host_addr;
		int host_addrlen;
		request req;
		response resp;
		int filter;
		bool is_listening_sock;
		struct kevent *change_ptr;
		int _method;
};


#endif