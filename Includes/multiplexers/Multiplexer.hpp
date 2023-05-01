#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP


#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include "../Client.hpp"

class Multiplexer
{
	private:
		virtual void setUpServerConnections(std::vector<Server *> servers) = 0;
		virtual void	monitoringLoop() = 0;
		virtual void    acceptConnections(Client *tempData) = 0;
		virtual void	read(Client *tempData) = 0;
		virtual void	write(Client *tempData) = 0;

	public :
		virtual void serve(std::vector<Server *> servers) = 0;
};

#endif