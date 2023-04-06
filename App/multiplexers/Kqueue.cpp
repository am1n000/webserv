#include "../../Includes/multiplexers/Kqueue.hpp"

int	Kqueue::kq = kqueue();
Kqueue::Kqueue() {}

Kqueue::~Kqueue() {}


void Kqueue::setUpServerConnections(std::vector<Server *> servers)
{
    if (kq == -1)
    {
		std::cerr << "error: kqueue" << std::endl;
        exit (1);
    }
	for (size_t i = 0; i < servers.size(); i++)
	{
		Client *server_data = new Client;
		server_data->setSockFd(servers[i]->createSocket());
		if (server_data->getSockFd() == -1)
			continue;
		if (servers[i]->bindSocket(server_data->getSockFd()))
			continue;
		if (servers[i]->listenToConnections(server_data->getSockFd()))
			continue;
		server_data->setIsListeningSock(1);
		server_data->server = servers[i];
		EV_SET(server_data->getChangePtr(), server_data->getSockFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, server_data);
		if (kevent(kq, server_data->getChangePtr(), 1, NULL, 0, NULL) != 0)
		{
			std::cerr << "error: kevent registration 1" << std::endl;
			continue;
		}
    }
}

void Kqueue::event_loop()
{
	while (1)
	{
		struct kevent events[MAX_EVENTS];
		int eventCount = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (eventCount == -1)
			std::cerr << "error: kevent monitoring" << std::endl;
		for (int j = 0; j < eventCount; j++)
		{
			Client *tempData = (Client *)events[j].udata;
			if (tempData->getIsListeningSock())
				acceptConnections(tempData);
			else
			{
				if (tempData->getFilter() == EVFILT_READ)
					read(tempData);
				else
					write(tempData);
				break;
			}
		}
	}
}

void Kqueue::acceptConnections(Client *clientData)
{
	int client_sock = accept(clientData->getSockFd(),
		reinterpret_cast<sockaddr *>(&(clientData->server->getHostAddr())),
		reinterpret_cast<socklen_t *>(&(clientData->server->getHostAddrlen())));
	if (client_sock == -1)
		std::cerr << "error: accept server " << std::endl;
	else
	{
		Client *u_data = new Client(client_sock, EVFILT_READ, 0);
		EV_SET(u_data->getChangePtr(), client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, u_data);
		if (kevent(kq, u_data->getChangePtr(), 1, NULL, 0, NULL) != 0)
			std::cerr << "error: kevent registration 2" << std::endl;
	}
}


void	Kqueue::read(Client *clientData)
{
	try
	{
		if (clientData->reading())
		{
			EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_READ,
					EV_DELETE, 0, 0, clientData);
			if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
			std::cerr << "error: kevent 3" << std::endl;
			EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_WRITE,
					EV_ADD | EV_ENABLE,0,0, clientData);
			if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
			std::cerr << "error: kevent 33" << std::endl;
			clientData->setFilter(EVFILT_WRITE);
		}

	}
	catch (std::exception &e)
	{
		EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_READ, EV_DELETE, 0, 0, clientData);
		kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL);
		close(clientData->getSockFd());
	}
}

void	Kqueue::write(Client *clientData)
{
	try
	{
		if (clientData->sending())
		{
			EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_WRITE, EV_DELETE, 0, 0, clientData);
			kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL);
			close(clientData->getSockFd());
			delete (clientData);
		}
	}
	catch (std::exception &e) //! to be modified according to every exception thrown
	{
		EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_WRITE, EV_DELETE, 0, 0, clientData);
		kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL);
		close(clientData->getSockFd());
	}
}


void Kqueue::serve(std::vector<Server *> servers)
{
	setUpServerConnections(servers);
	event_loop();
}