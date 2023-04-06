#ifndef KQUEUE_HPP
#define KQUEUE_HPP


#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include "../Client.hpp"

class Kqueue
{
	private:
		Kqueue();
		static int kq;
		static void setUpServerConnections(std::vector<Server *> servers);
		static void	event_loop();
		static void acceptConnections(Client *tempData);
		static void	read(Client *tempData);
		static void	write(Client *tempData);

	public :
		~Kqueue();
		static void serve(std::vector<Server *> servers);
};

#endif