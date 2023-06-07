#include "../../Includes/multiplexers/Poll.hpp"
#include <cstdlib>

Poll::Poll() {}

Poll::~Poll() {}

int                     Poll::position;
std::vector<pollfd>     Poll::pollFds;
std::vector<Client *>   Poll::clientsData;
Poll*	Poll::instance = NULL;


Poll*	Poll::getInstance()
{
	if (instance == NULL)
		instance = new Poll;
	return (instance);
}

void	Poll::deleteInstance()
{
	if (Poll::instance != NULL)
		delete (Poll::instance);
}
void Poll::setUpServerConnections()
{
	std::vector<Server*> &servers = Config::get()->getServers();
	std::map<std::string, std::string> hostPort;
	for (size_t i = 0; i < servers.size(); i++)
	{
		Client *server_data = new Client;
		struct pollfd	pfd;
		server_data->setSockFd(servers[i]->createSocket());
		if (server_data->getSockFd() == -1)
		{
			delete(server_data);
			continue;
		}
		if (servers[i]->bindSocket(server_data->getSockFd()))
		{
			std::cerr << "error :bind" << std::endl;
			exit(1);
		}
		if (servers[i]->listenToConnections(server_data->getSockFd()))
		{
			delete(server_data);
			continue;
		}
		hostPort[servers[i]->getHost()] = servers[i]->getPort();
		server_data->setIsListeningSock(1);
		server_data->server = servers[i];
		clientsData.push_back(server_data);
		
		pfd.fd = server_data->getSockFd();
		pfd.events = POLLIN;
		pollFds.push_back(pfd);
	}
}
void Poll::monitoringLoop()
{
	while (true)
	{
		int ready_num = poll(&pollFds[0], pollFds.size() , 5000);
		if (ready_num < 0)
		{
			std::cerr << "error : poll  " <<  std::endl;
			exit (1);
		}
		for (size_t i = 0; i < pollFds.size() && ready_num > 0; i++)
		{
			position = i;
			if (pollFds[i].revents & POLLIN)
			{
				if (clientsData[i]->getIsListeningSock())
					acceptConnections(clientsData[i]);
				else
					read(clientsData[i]);
				ready_num--;
			}
			else if (pollFds[i].revents & POLLOUT)
			{
				write(clientsData[i]);
				ready_num--;
			}
		}
	}
}

void Poll::acceptConnections(Client *clientData)
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
		struct pollfd pfd;
		pfd.fd = client_sock;
		pfd.events = POLLIN;
		pollFds.push_back(pfd);
	}
}


void	Poll::read(Client *clientData)
{			
	try
	{
		if (clientData->reading())
			pollFds[position].events = POLLOUT;
	}
	catch(statusCodeExceptions &e)
	{
    std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
    helpers::displayStatusCodePage(e, clientData->getSockFd(), pages[e.getValue()]);
		close (clientData->getSockFd());
		delete (clientsData[position]);
		clientsData.erase(clientsData.begin() + position);
		pollFds.erase(pollFds.begin() + position);
	}
}

void	Poll::write(Client *clientData)
{
	try
	{
		if (clientData->sending())
		{
			close (clientData->getSockFd());
			delete (clientsData[position]);
			clientsData.erase(clientsData.begin() + position);
			pollFds.erase(pollFds.begin() + position);
		}
	}
	catch(statusCodeExceptions &e)
	{		
    std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
    helpers::displayStatusCodePage(e, clientData->getSockFd(), pages[e.getValue()]);
		close (clientData->getSockFd());
		delete (clientsData[position]);
		clientsData.erase(clientsData.begin() + position);
		pollFds.erase(pollFds.begin() + position);
	}
}


void Poll::serve()
{
	setUpServerConnections();
	monitoringLoop();
}
