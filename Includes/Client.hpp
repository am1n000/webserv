#ifndef CLIENT_HPP

#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#define MAX_EVENTS 1000

class Client
{
	private : //.variables
		int 				_sockFd;
		int					_filter;
		int 				_hostAddrlen;
		bool				_isListeningSock;
		bool				_postFileCreated;
		struct kevent		*_changePtr;
		struct sockaddr_in	_hostAddr;

	public :
		Client();
		Client(int sockfd, int filtr, bool listen_sock);
		~Client();
		Request 			*req;
		Response			*resp;

		bool				sending(int kq);
		void				reading(int kq);
    void prepareResponse();

		//.getters
		int					getSockFd();
		int					getFilter();
		int&				getHostAddrlen();
		bool				getIsListeningSock();
		bool				getpostFileCreated();
		struct kevent*		getChangePtr();
		struct sockaddr_in&	getHostAddr();

		//.setters
		void				setSockFd(int sockFd);
		void				setFilter(int filter);
		void				setHostAddrlen(int hostAddrlen);
		void				setIsListeningSock(int isListeningSock);
		void				setpostFileCreated(bool postFileCreated);
		void				setChangePtr();
		void				setHostAddr(int port);
};


#endif
