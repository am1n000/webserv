#ifndef CLIENT_HPP
#define CLIENT_HPP


#ifdef	__APPLE__
	#include <sys/event.h>
#elif	__linux__
	#include <sys/epoll.h>
#endif
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define MAX_EVENTS 1000

class Client
{
	private :
		int 				_sockFd;
		bool				_isListeningSock;
		bool				_postFileCreated;
		#ifdef __APPLE__
			struct kevent		*_changePtr;
			struct kevent		*_timeoutChangePtr;
		#endif
		void	checkServerByName();

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
		#ifdef __APPLE__
			struct kevent*		getChangePtr();
			struct kevent*		getTimeoutChangePtr();
		#endif

		//.setters
		void				setSockFd(int sockFd);
		void				setIsListeningSock(int isListeningSock);
		void				setpostFileCreated(bool postFileCreated);
		#ifdef __APPLE__
			void				setChangePtr();
			void				setTimeoutChangePtr();
		#endif
};


#endif
