#ifndef CLENTPOLL_HPP

#define CLENTPOLL_HPP

#include "Request.hpp"
#include "Response.hpp"
#include <poll.h>
#define MAX_EVENTS 1000

class clientPoll
{
	private : //.variables
		int 				_sockFd;
		int 				_hostAddrlen;
		int 				_postFileCreated;
		bool				_isServerSock;
		struct sockaddr_in	_hostAddr;

	public :
		clientPoll();
		clientPoll(int sockfd, bool listen_sock);
		~clientPoll();
		Request 			*req;
		Response			*resp;

		void				prepare_response();
		void				reading(std::vector<struct pollfd> *poll_fds, int pos);
		bool				sending();

		//.getters
		int					getSockFd();
		int&				getHostAddrlen();
		bool				getIsServerSock();
		bool				getpostFileCreated();
		struct sockaddr_in&	getHostAddr();

		//.setters
		void				setSockFd(int sockFd);
		void				setHostAddrlen(int hostAddrlen);
		void				setIsServerSock(int isListeningSock);
		void				setHostAddr(int port);
		void				setpostFileCreated(bool postFileCreated);
};


#endif