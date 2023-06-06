#include "../../Includes/multiplexers/Kqueue.hpp"

#ifdef __APPLE__
	int	Kqueue::kq = kqueue();
	Kqueue*	Kqueue::instance = nullptr;

	Kqueue::Kqueue() {}

	Kqueue::~Kqueue() {}

	Kqueue*	Kqueue::getInstance()
	{
		if (Kqueue::instance == nullptr)
			Kqueue::instance = new Kqueue;
		return (Kqueue::instance);
	}

	void Kqueue::setUpServerConnections()
	{
		std::vector<Server*> &servers = Config::get()->getServers();
		std::map<std::string, std::string> hostPort;
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
				if (hostPort[servers[i]->getHost()] == servers[i]->getPort())
				{
					delete (server_data);
					continue;
				}
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
			server_data->server = Config::get()->getServers()[i];
			EV_SET(server_data->getChangePtr(), server_data->getSockFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, server_data);
			if (kevent(kq, server_data->getChangePtr(), 1, NULL, 0, NULL) != 0)
			{
				std::cerr << "error: kevent registration 1" << std::endl;
				delete (server_data);
				continue;
			}
		}
	}

	void Kqueue::monitoringLoop()
	{
		struct timespec timeout;
		timeout.tv_sec = 5;
		timeout.tv_nsec = 0;
		while (1)
		{
			struct kevent events[MAX_EVENTS];
			int eventCount = kevent(kq, NULL, 0, events, MAX_EVENTS, &timeout);
			if (eventCount == -1)
				std::cerr << "error: kevent monitoring" << std::endl;
			for (int j = 0; j < eventCount; j++)
			{
				Client *tempData = (Client *)events[j].udata;
				if (tempData->getIsListeningSock())
					acceptConnections(tempData);
				else
				{ 
					if (events[j].filter == EVFILT_READ)
						this->read(tempData);
					else if (events[j].filter == EVFILT_WRITE)
						this->write(tempData);
					else
						this->timeout(tempData);
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
			std::cerr << "error: accept server " << clientData->server->getServerName() <<  std::endl;
		else
		{
			Client *u_data = new Client(client_sock, 0);
			u_data->server = clientData->server;
			u_data->req->setServer(clientData->server);
			EV_SET(u_data->getChangePtr(), client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, u_data);
			EV_SET(u_data->getTimeoutChangePtr(), client_sock, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, 5, u_data);
			if (kevent(kq, u_data->getChangePtr(), 1, NULL, 0, NULL) != 0 
				|| kevent(kq, u_data->getTimeoutChangePtr(), 1, NULL, 0, NULL) != 0)
			{
				std::cerr << "error: kevent registration 2" << std::endl;
				helpers::InternalServerError(client_sock);
				delete (u_data);
			}
		}
	}


	void	Kqueue::read(Client *clientData)
	{
		try
		{
			if (clientData->reading())
			{
				EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_READ, EV_DELETE, 0, 0, clientData);
				if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "error: kevent 3" << std::endl;
					throw (InternalServerErrorException());
				}

				EV_SET(clientData->getTimeoutChangePtr(), clientData->getSockFd(), EVFILT_TIMER, EV_DELETE, 0, 0, clientData);
				if (kevent(kq, clientData->getTimeoutChangePtr(), 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "error: kevent 3" << std::endl;
					throw (InternalServerErrorException());
				}

				EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE,0,0, clientData);
				if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "error: kevent 3" << std::endl;
					throw (InternalServerErrorException());
				}
			}

		}
		catch (statusCodeExceptions &e)
		{
			std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
			helpers::displayStatusCodePage(e, clientData->getSockFd(), 0, pages[e.getValue()]);
			EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_READ, EV_DELETE, 0, 0, clientData);
			if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
			{
					std::cerr << "error: kevent 4" << std::endl;
					helpers::InternalServerError(clientData->getSockFd());
			}
			close(clientData->getSockFd());
			delete (clientData);

		}
	}

	void	Kqueue::write(Client *clientData)
	{
		try
		{
			if (clientData->sending())
			{
				EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_WRITE, EV_DELETE, 0, 0, clientData);
				if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "error: kevent 5" << std::endl;
					throw(InternalServerErrorException());
				}
				close(clientData->getSockFd());
				delete (clientData);
			}
		}
		catch (statusCodeExceptions &e)
		{	
      		std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
    		helpers::displayStatusCodePage(e, clientData->getSockFd(), 0, pages[e.getValue()]);
			EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_WRITE, EV_DELETE, 0, 0, clientData);
			if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1)
			{
				std::cerr << "error: kevent 6" << std::endl;
				helpers::InternalServerError(clientData->getSockFd());
			}
			close(clientData->getSockFd());
			delete (clientData);
		}
	}

	void	Kqueue::timeout(Client *clientData)
	{
		try
		{
			if (!clientData->req->isRequestCompleted())
				throw(RequestTimeoutException());
		}
		catch(statusCodeExceptions &e)
		{
      		std::map<std::string, std::string> pages = clientData->req->getServer()->getErrorPages();
			helpers::displayStatusCodePage(e, clientData->getSockFd(), 0, pages[e.getValue()]);
			EV_SET(clientData->getChangePtr(), clientData->getSockFd(), EVFILT_READ, EV_DELETE, 0, 0, clientData);
			EV_SET(clientData->getTimeoutChangePtr(), clientData->getSockFd(), EVFILT_TIMER, EV_DELETE, 0, 0, clientData);
			if (kevent(kq, clientData->getChangePtr(), 1, NULL, 0, NULL) == -1
				|| kevent(kq, clientData->getTimeoutChangePtr(), 1, NULL, 0, NULL) == -1)
			{
				std::cerr << "error: kevent 7" << std::endl;
				helpers::InternalServerError(clientData->getSockFd());
			}
			close(clientData->getSockFd());
			delete (clientData);
		}
		
	}


	void Kqueue::serve()
	{
		if (kq == -1)
		{
			std::cerr << "error: kqueue" << std::endl;
			exit(1);
		}
		setUpServerConnections();
		monitoringLoop();
	}

#endif
