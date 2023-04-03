#include "Includes/Client.hpp"


class servero
{
	public:
		int _port;
		servero(int port) : _port(port) {};
		~servero() {};
};

int event_loop(int kq, std::vector<Client*> servers_data)
{
	while (1)
	{
		struct kevent events[MAX_EVENTS];
		int nevents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (nevents == -1)
			std::cerr << "error: kevent monitoring" << std::endl;
		for (int j = 0; j < nevents; j++)
		{
			Client *temp_data = (Client *)events[j].udata;
			for (size_t i = 0; i < servers_data.size(); i++)
			{
				if (temp_data->getSockFd() == servers_data[i]->getSockFd())
				{
					int client_sock = accept(servers_data[i]->getSockFd(), reinterpret_cast<sockaddr *>(&(servers_data[i]->getHostAddr())), reinterpret_cast<socklen_t *>(&(servers_data[i]->getHostAddrlen())));
					if (client_sock == -1)
						std::cerr << "error: accept server " << std::endl;
					else
					{
						Client *u_data = new Client(client_sock, EVFILT_READ, 0);
						EV_SET(u_data->getChangePtr(), client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, u_data);
						if (kevent(kq, u_data->getChangePtr(), 1, NULL, 0, NULL) != 0)
							std::cerr << "error: kevent registration 2" << std::endl;
					}
					break;
				}
				else if (!temp_data->getIsListeningSock())
				{
					if (temp_data->getFilter() == EVFILT_READ)
					{
						temp_data->reading(kq);
					}
					else
					{
						if (temp_data->sending(kq))
							delete (temp_data);
					}
					break;
				}
			}
		}
	}

}

void serving(std::vector<servero> servers)
{
	int kq = kqueue();
	std::vector<Client *> servers_data;
	if (kq == -1)
		std::cerr << "error: kqueue" << std::endl;
	for (size_t j = 0; j < servers.size(); j++)
	{
		Client *server_data = new Client;
		server_data->setSockFd(-2);
		if (server_data->getSockFd() == -1)
		{
			std::cerr << "error: socket" << std::endl;
			continue;
		}
		fcntl(server_data->getSockFd(), F_SETFL, O_NONBLOCK); // if not the connection will block
		server_data->setHostAddrlen(sizeof(server_data->getHostAddr()));
		server_data->setHostAddr(servers[j]._port);
		// .allowing the socket to be reusable..................................
		int opt = 1;
		if (setsockopt(server_data->getSockFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "error: setsockopt" << std::endl;
			continue;
		}
		opt = 1;
		if (setsockopt(server_data->getSockFd(), SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "error: setsockopt" << std::endl;
			continue;
		}
		// .binding the socket with an adress...................................
		if (bind(server_data->getSockFd(), reinterpret_cast<struct sockaddr *>(&(server_data->getHostAddr())), server_data->getHostAddrlen()) != 0) //? sockaddr is just an interface, is a sort of polymorphism
		{
			std::cerr << "error: bind" << std::endl;
			continue;
		}
		if (listen(server_data->getSockFd(), SOMAXCONN) == -1)
		{
			std::cerr << "error: listen" << std::endl;
			continue;
		}
		// server_data->getChangePtr() = new struct kevent;
		EV_SET(server_data->getChangePtr(), server_data->getSockFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, server_data);
		if (kevent(kq, server_data->getChangePtr(), 1, NULL, 0, NULL) != 0)
		{
			std::cerr << "error: kevent registration 1" << std::endl;
			continue;
		}
		servers_data.push_back(server_data);
	}
	event_loop(kq, servers_data);
}

int main ()
{
	servero s1(8080);
	servero s2(8081);
	servero s3(8082);
	std::vector<servero> servers;
	servers.push_back(s1);
	servers.push_back(s2);
	servers.push_back(s3);

	serving(servers);
}
