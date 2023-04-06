#ifndef CLENTSELECT_HPP

#define CLENTSELECT_HPP

#include "Request.hpp"
#include "Response.hpp"
#define MAX_EVENTS 1000

class clientSelect
{
	private : //.variables
		int 				_sockFd;
		int 				_hostAddrlen;
		int 				_postFileCreated;
		bool				_isServerSock;
		struct sockaddr_in	_hostAddr;

	public :
		clientSelect();
		clientSelect(int sockfd, bool listen_sock);
		~clientSelect();
		Request 			*req;
		Response			*resp;

		void				prepare_response();
		void				reading(fd_set *read_master, fd_set *write_master);
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