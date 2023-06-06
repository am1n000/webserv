#ifndef SELECT_HPP
#define SELECT_HPP

#include <sys/select.h>
#include "Multiplexer.hpp"

class Select : public Multiplexer
{
	private:
        static  int 					fd_max;
		static	int						erasePosition;
        static  fd_set 					readMaster;
        static  fd_set 					writeMaster;
        static  std::vector<Client *>	clientsData;
		static	Select*					instance;

		Select();

		void 	setUpServerConnections();
		void	monitoringLoop();
		void 	acceptConnections(Client *tempData);
		void	read(Client *tempData);
		void	write(Client *tempData);

	public :

		~Select();
		static	Select* getInstance();
		static	Select* deleteInstance();
		void 			serve();
};

#endif