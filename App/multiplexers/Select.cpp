#include "../../Includes/multiplexers/Select.hpp"
#include <cstdlib>

Select::Select() {}

Select::~Select() {}


int                     Select::fd_max;
int                     Select::erasePosition;
fd_set                  Select::readMaster;
fd_set                  Select::writeMaster;
std::vector<Client *>   Select::clientsData;
Select*					Select::instance = NULL;


Select*	Select::getInstance()
{
	if (instance == NULL)
		instance = new Select;
	return (instance);
}
void	Select::deleteInstance()
{
	if (Select::instance != NULL)
		delete (Select::instance);
}

void Select::setUpServerConnections()
{
	std::vector<Server*> &servers = Config::get()->getServers();
	std::map<std::string, std::string> hostPort;
	FD_ZERO(&readMaster);
	for (size_t i = 0; i < servers.size(); i++)
	{
		Client *server_data = new Client;
		server_data->setSockFd(servers[i]->createSocket());
		if (server_data->getSockFd() == -1)
		{
			delete (server_data);
			continue;
		}
		if (servers[i]->bindSocket(server_data->getSockFd()))
		{
			std::cerr << "error :bind" << std::endl;
			exit(1);
		}
		if (servers[i]->listenToConnections(server_data->getSockFd()))
		{
			delete (server_data);
			continue;
		}
		hostPort[servers[i]->getHost()] = servers[i]->getPort();
		server_data->setIsListeningSock(1);
		server_data->server = servers[i];
		FD_SET(server_data->getSockFd(), &readMaster);
		clientsData.push_back(server_data);
		fd_max = server_data->getSockFd();
	}
}
void Select::monitoringLoop()
{
	FD_ZERO(&writeMaster);
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	while (true)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		read_fds = readMaster;
		write_fds = writeMaster;
		int ready_num = select(fd_max + 1, &read_fds, &write_fds, NULL, &timeout);
		if (ready_num < 0)
		{
			std::cerr << "error : select  " <<  std::endl;
			exit (1);
		}
		for (size_t i = 0; i < clientsData.size() && ready_num > 0; i++)
		{
			erasePosition = i;
			if (FD_ISSET(clientsData[i]->getSockFd(), &read_fds))
			{
				if (clientsData[i]->getIsListeningSock())
					acceptConnections(clientsData[i]);
				else
					read(clientsData[i]);
				ready_num--;
			}
			else if (FD_ISSET(clientsData[i]->getSockFd(), &write_fds))
			{
				write(clientsData[i]);
				ready_num--;
			}
			while (fd_max > 2)
			{// finding the new max after deleting the old max
				if (FD_ISSET(fd_max, &readMaster) || FD_ISSET(fd_max, &writeMaster))
					break;
				fd_max--;
			}
		}
	}
}

void Select::acceptConnections(Client *clientData)
{				
    int client_sock = accept(clientData->getSockFd(),
        reinterpret_cast<sockaddr *>(&(clientData->server->getHostAddr())),
        reinterpret_cast<socklen_t *>(&(clientData->server->getHostAddrlen())));
    if (client_sock == -1)
        std::cerr << "error: accept server " << std::endl;
    else
    {
        Client *new_client = new Client(client_sock, 0);
		new_client->server = clientData->server;
    	new_client->req->setServer(clientData->server);
        clientsData.push_back(new_client);
        FD_SET(client_sock, &readMaster);
        if (client_sock > fd_max)
            fd_max = client_sock;
    }
}


void	Select::read(Client *clientData)
{					
	try
	{
		if (clientData->reading())
		{
			FD_CLR(clientData->getSockFd(), &readMaster);
			FD_SET(clientData->getSockFd(), &writeMaster);
		}
	}
	catch(statusCodeExceptions &e)
	{
		std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
		helpers::displayStatusCodePage(e, clientData->getSockFd(), pages[e.getValue()]);
		FD_CLR(clientData->getSockFd(), &readMaster);
		close (clientData->getSockFd());
		std::cerr << "-------------   " << clientData->getSockFd() <<  std::endl;
		std::cerr << "read :  " << erasePosition << std::endl;
		delete (clientsData[erasePosition]);
		clientsData.erase(clientsData.begin() + erasePosition);
	}
}

void	Select::write(Client *clientData)
{

	try
	{
		if (clientData->sending())
		{
			FD_CLR(clientData->getSockFd(), &writeMaster);
			close (clientData->getSockFd());
			delete (clientsData[erasePosition]);
			clientsData.erase(clientsData.begin() + erasePosition);
		}
	}
	catch(statusCodeExceptions &e)
	{
		std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
		helpers::displayStatusCodePage(e, clientData->getSockFd(), pages[e.getValue()]);
		FD_CLR(clientData->getSockFd(), &writeMaster);
		close (clientData->getSockFd());
		delete (clientsData[erasePosition]);
		clientsData.erase(clientsData.begin() + erasePosition);
	}
}


void Select::serve()
{
	setUpServerConnections();
	monitoringLoop();
}
