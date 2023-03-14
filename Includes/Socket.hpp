#ifndef SOCKET_HPP

#define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


class sock
{
	public:
		sock();
		~sock();
		sock(sock const &obj);
		sock& operator=(sock const &obj);
		
		int create_sock();
};


#endif