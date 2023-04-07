#ifndef POLL_HPP
#define POLL_HPP

#include <poll.h>
#include "../Client.hpp"

class Poll
{
	private:
		static	int							position;
        static  std::vector<Client *>		clientsData;
		static	std::vector<struct pollfd>	pollFds;
		Poll();
		static void setUpServerConnections(std::vector<Server *> servers);
		static void	MonitoringLoop();
		static void acceptConnections(Client *tempData);
		static void	read(Client *tempData);
		static void	write(Client *tempData);

	public :
		~Poll();
		static void serve(std::vector<Server *> servers);
};

#endif