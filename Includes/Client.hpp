#ifndef CLIENT_HPP

#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define MAX_EVENTS 1000

class Client
{
	private : //.variables
		int 				_sockFd;
		bool				_isListeningSock;
		bool				_postFileCreated;
		struct kevent		*_changePtr;

	public :
		Client();
		Client(int sockfd, bool listen_sock);
		~Client();
		Request 			*req;
		Response			*resp;
		Server				*server;

		bool				sending();
		bool				reading();

		//.getters
		int					getSockFd();
		bool				getIsListeningSock();
		bool				getpostFileCreated();
		struct kevent*		getChangePtr();

		//.setters
		void				setSockFd(int sockFd);
		void				setIsListeningSock(int isListeningSock);
		void				setpostFileCreated(bool postFileCreated);
		void				setChangePtr();
};


#endif
