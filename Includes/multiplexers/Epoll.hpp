#ifndef EPOLL_HPP
#define EPOLL_HPP


#include <sys/time.h>
#include <iostream>
#include "Multiplexer.hpp"

class Epoll : public Multiplexer
{
	private:
		static int 		ep;
		static	Epoll*	instance;

		Epoll();
		
		void 	setUpServerConnections();
		void	monitoringLoop();
		void 	acceptConnections(Client *tempData);
		void	read(Client *tempData);
		void	write(Client *tempData);


	public :
		~Epoll();

		static	Epoll* getInstance();
		void 			serve();
};

#endif