#ifndef KQUEUE_HPP
#define KQUEUE_HPP


#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include "Multiplexer.hpp"

class Kqueue : public Multiplexer
{
	private:
		static int 		kq;
		static	Kqueue*	instance;

		Kqueue();
		
		void 	setUpServerConnections(std::vector<Server *> servers);
		void	monitoringLoop();
		void 	acceptConnections(Client *tempData);
		void	read(Client *tempData);
		void	write(Client *tempData);


	public :
		~Kqueue();

		static	Kqueue* getInstance();
		void 			serve(std::vector<Server *> servers);
};

#endif