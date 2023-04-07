#ifndef SELECT_HPP
#define SELECT_HPP

#include <sys/select.h>
#include "../Client.hpp"

class Select
{
	private:
        static  int 					fd_max;
		static	int						erasePosition;
        static  fd_set 					readMaster;
        static  fd_set 					writeMaster;
        static  std::vector<Client *>	clientsData;
		Select();
		static void setUpServerConnections(std::vector<Server *> servers);
		static void	MonitoringLoop();
		static void acceptConnections(Client *tempData);
		static void	read(Client *tempData);
		static void	write(Client *tempData);

	public :
		~Select();
		static void serve(std::vector<Server *> servers);
};

#endif