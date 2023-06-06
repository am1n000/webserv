#ifndef KQUEUE_HPP
#define KQUEUE_HPP


#include <sys/time.h>
#include <iostream>
#include "Multiplexer.hpp"

class Kqueue : public Multiplexer
{
	private:
		static int 		kq;
		static	Kqueue*	instance;

		Kqueue();
		
		void 	setUpServerConnections();
		void	monitoringLoop();
		void 	acceptConnections(Client *tempData);
		void	read(Client *tempData);
		void	write(Client *tempData);
		void	timeout(Client *tempData);


	public :
		~Kqueue();

		static	Kqueue* getInstance();
		static	Kqueue* deleteInstance();
		void 			serve();
};

#endif