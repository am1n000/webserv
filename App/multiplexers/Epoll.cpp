#include "../../Includes/multiplexers/Epoll.hpp"
#include <cstdlib>

#ifdef __linux__
	int	Epoll::ep = epoll_create(16);
	Epoll*	Epoll::instance = NULL;

	Epoll::Epoll() {}

	Epoll::~Epoll() {}

	Epoll*	Epoll::getInstance()
	{
		if (Epoll::instance == NULL)
			Epoll::instance = new Epoll;
		return (Epoll::instance);
	}

	void	Epoll::deleteInstance()
	{
		if (Epoll::instance != NULL)
			delete (Epoll::instance);
	}

	void Epoll::setUpServerConnections()
	{
		std::vector<Server*> &servers = Config::get()->getServers();
		std::map<std::string, std::string> hostPort;
		for (size_t i = 0; i < servers.size(); i++)
		{
			Client *serverData = new Client;
			serverData->setSockFd(servers[i]->createSocket());
			if (serverData->getSockFd() == -1)
			{
				delete (serverData);			
				continue;
			}
			if (servers[i]->bindSocket(serverData->getSockFd()))
			{
				std::cerr << "error :bind" << std::endl;
				exit(1);
			}
			if (servers[i]->listenToConnections(serverData->getSockFd()))
			{
				delete (serverData);
				continue;
			}
			hostPort[servers[i]->getHost()] = servers[i]->getPort();
			serverData->setIsListeningSock(1);
			serverData->server = Config::get()->getServers()[i];
			struct epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = serverData->getSockFd();
			event.data.ptr = serverData;
			if (epoll_ctl(ep, EPOLL_CTL_ADD, serverData->getSockFd(), &event) == -1)
			{
				std::cerr << "error: epoll_ctl" << std::endl;
				delete (serverData);
				continue;
			}
		}
	}

	void Epoll::monitoringLoop()
	{
		while (1)
		{
			struct epoll_event events[MAX_EVENTS];
			int eventCount = epoll_wait(ep, events, MAX_EVENTS, 5000);
			if (eventCount == -1)
				std::cerr << "error: epoll_wait" << std::endl;
			for (int j = 0; j < eventCount; j++)
			{
				Client *tempData = (Client *)events[j].data.ptr;
				if (tempData->getIsListeningSock())
					acceptConnections(tempData);
				else
				{ 
					if (events[j].events == EPOLLIN)
						read(tempData);
					else
						write(tempData);
				}
			}
		}
		close (ep);
	}

	void Epoll::acceptConnections(Client *clientData)
	{
		int client_sock = accept(clientData->getSockFd(),
			reinterpret_cast<sockaddr *>(&(clientData->server->getHostAddr())),
			reinterpret_cast<socklen_t *>(&(clientData->server->getHostAddrlen())));
		if (client_sock == -1)
			std::cerr << "error: accept server " << std::endl;
		else
		{
			Client *ptr = new Client(client_sock, 0);
			ptr->server = clientData->server;
			ptr->req->setServer(clientData->server);
			struct epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = ptr->getSockFd();
			event.data.ptr = ptr;
			if (epoll_ctl(ep, EPOLL_CTL_ADD, ptr->getSockFd(), &event) == -1)
			{
				std::cerr << "error: epoll_ctl 2" << std::endl;
				helpers::InternalServerError(client_sock);
				delete (ptr);
			}
		}
	}


	void	Epoll::read(Client *clientData)
	{
		try
		{
			if (clientData->reading())
			{
				struct epoll_event event;
				event.events = EPOLLOUT;
				event.data.fd = clientData->getSockFd();
				event.data.ptr = clientData;
				if (epoll_ctl(ep, EPOLL_CTL_MOD, clientData->getSockFd(), &event) == -1)
				{
					std::cerr << "error: epoll_ctl write" << std::endl;
					throw (InternalServerErrorException());
				}
			}

		}
		catch (statusCodeExceptions &e)
		{
			std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
			helpers::displayStatusCodePage(e, clientData->getSockFd(), pages[e.getValue()]);
			if (epoll_ctl(ep, EPOLL_CTL_DEL, clientData->getSockFd(), NULL) == -1)
			{
				std::cerr << "error: epoll_ctl deletion" << std::endl;
				helpers::InternalServerError(clientData->getSockFd());
			}
			close(clientData->getSockFd());
			delete (clientData);

		}
	}

	void	Epoll::write(Client *clientData)
	{
		try
		{
			if (clientData->sending())
			{
				if (epoll_ctl(ep, EPOLL_CTL_DEL, clientData->getSockFd(), NULL) == -1)
				{
					std::cerr << "error: epoll_ctl deletion" << std::endl;
					throw(InternalServerErrorException());
				}
				close(clientData->getSockFd());
				delete (clientData);
			}
		}
		catch (statusCodeExceptions &e)
		{	
			std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
			helpers::displayStatusCodePage(e, clientData->getSockFd(), pages[e.getValue()]);
			if (epoll_ctl(ep, EPOLL_CTL_DEL, clientData->getSockFd(), NULL) == -1)
			{
				std::cerr << "error: epoll_ctl deletion" << std::endl;
				helpers::InternalServerError(clientData->getSockFd());
			}
			close(clientData->getSockFd());
			delete (clientData);
		}
	}


	void Epoll::serve()
	{
		if (ep == -1)
		{
			std::cerr << "error: Epoll" << std::endl;
			exit (1);
		}
		setUpServerConnections();
		monitoringLoop();
	}

#endif
