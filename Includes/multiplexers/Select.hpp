#ifndef SELECT_HPP
#define SELECT_HPP


#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include "../Client.hpp"

class Select
{
	private:
		Select();
		static int kq;
		static void setUpServerConnections(std::vector<Server *> servers);
		static void	event_loop();
		static void acceptConnections(Client *tempData);
		static void	read(Client *tempData);
		static void	write(Client *tempData);

	public :
		~Select();
		static void serve(std::vector<Server *> servers);
};

#endif