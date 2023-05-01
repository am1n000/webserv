#ifndef POLL_HPP
#define POLL_HPP

#include <poll.h>
#include "Multiplexer.hpp"

class Poll : public Multiplexer
{
	private:
		static	int							position;
        static  std::vector<Client *>		clientsData;
		static	std::vector<struct pollfd>	pollFds;
		static	Poll*						instance;

		Poll();

		void 	setUpServerConnections();
		void	monitoringLoop();
		void 	acceptConnections(Client *tempData);
		void	read(Client *tempData);
		void	write(Client *tempData);

	public :

		~Poll();
		static	Poll*	getInstance();
		void 			serve();
};

#endif