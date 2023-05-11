#include "../../Includes/multiplexers/Poll.hpp"

Poll::Poll() {}

Poll::~Poll() {}

int                     Poll::position;
std::vector<pollfd>     Poll::pollFds;
std::vector<Client *>   Poll::clientsData;
Poll*	Poll::instance = nullptr;


Poll*	Poll::getInstance()
{
	if (instance == nullptr)
		instance = new Poll;
	return (instance);
}
void Poll::setUpServerConnections()
{
	std::vector<Server*> &servers = Config::get()->getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		Client *server_data = new Client;
		struct pollfd	pfd;
		server_data->setSockFd(servers[i]->createSocket());
		if (server_data->getSockFd() == -1)
			continue;
		if (servers[i]->bindSocket(server_data->getSockFd()))
			continue;
		if (servers[i]->listenToConnections(server_data->getSockFd()))
			continue;
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
		int ready_num = poll(&pollFds[0], pollFds.size() , -1);
		if (ready_num < 0)
		{
			std::cerr << "error : poll  " <<  std::endl;
			exit (1);
		}
		for (size_t i = 0; i < pollFds.size() && ready_num > 0; i++)
		{
			// std::cout << pollFds.size() << std::endl;
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
		new_client->setPendingSize(1024);
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
	catch(std::exception &e)
	{
		close (clientData->getSockFd());
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
			clientsData.erase(clientsData.begin() + position);
			pollFds.erase(pollFds.begin() + position);
		}
	}
	catch(std::exception &e)
	{
		close (clientData->getSockFd());
		clientsData.erase(clientsData.begin() + position);
		pollFds.erase(pollFds.begin() + position);
	}
}


void Poll::serve()
{
	setUpServerConnections();
	monitoringLoop();
}