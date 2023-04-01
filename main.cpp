#include "Includes/Client.hpp"
#include "Includes/ClientSelect.hpp"


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
			for (size_t i = 0; i < servers_data.size(); i++) //! you don't need to loop just check isServerSock variable
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


void main_loop(std::map<int, clientSelect *> clients_data, fd_set read_master, int fd_max)
{
	fd_set write_master;
	FD_ZERO(&write_master);
	fd_set read_fds;
	fd_set write_fds;
	int pre_fd_max = fd_max - 1;
	while (true)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		read_fds = read_master;
		write_fds = write_master;
		int ready_num = select((--(clients_data.end()))->first + 1, &read_fds, &write_fds, NULL, NULL);
		if (ready_num < 0)
		{
			std::cerr << "error : select  " << errno <<  std::endl;
			exit (1);
		}
		for (int i = 3; i <= fd_max; i++) //! decrement the ready_num each time 
		{
			std::map<int, clientSelect *>::iterator it = clients_data.find(i);
			if (FD_ISSET(i, &read_fds))
			{
				if (it->second->getIsServerSock())
				{
					int client_sock = accept(it->second->getSockFd(), reinterpret_cast<sockaddr *>(&(it->second->getHostAddr())),
						reinterpret_cast<socklen_t *>(&(it->second->getHostAddrlen())));
					if (client_sock == -1)
						std::cerr << "error: accept server " << std::endl;
					else
					{
						clientSelect *new_client = new clientSelect(client_sock, 0);
						clients_data.insert(std::make_pair(client_sock, new_client));
						FD_SET(client_sock, &read_master);
						if (client_sock > fd_max)
						{
							pre_fd_max = fd_max;
							fd_max = client_sock;
						}
					}
				}
				else
				{
					try {
						it->second->reading(&read_master, &write_master);
					}
					catch(std::exception &e)
					{
						FD_CLR(it->first, &read_master);
						close (it->first);
						clients_data.erase(it->first);
					}
				}
			}
			else if (FD_ISSET(i, &write_fds))
			{
				bool finito;
				try
				{
					finito = it->second->sending();
				}
				catch(std::exception &e)
				{
					close (it->first);
					FD_CLR(it->first, &write_master);
					clients_data.erase(it->first);
				}
				if (finito)
				{
					close(it->first);
					FD_CLR(it->first, &write_master);
					clients_data.erase(it->first);
				}
			}
		}
	
	}

}

void select_module(std::vector<servero> servers)
{
    fd_set master_read;
	FD_ZERO(&master_read);
	int	fd_max;
	std::map<int, clientSelect *> servers_data;
    for (size_t j = 0; j < servers.size(); j++)
	{
		clientSelect *server_data = new clientSelect;
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
		if (bind(server_data->getSockFd(), reinterpret_cast<struct sockaddr *>(&(server_data->getHostAddr())),
			server_data->getHostAddrlen()) != 0) //? sockaddr is just an interface, is a sort of polymorphism
		{
			std::cerr << "error: bind" << std::endl;
			continue;
		}
		if (listen(server_data->getSockFd(), SOMAXCONN) == -1)
		{
			std::cerr << "error: listen" << std::endl;
			continue;
		}
		server_data->setIsServerSock(1);
		FD_SET(server_data->getSockFd(), &master_read);
		fd_max = server_data->getSockFd();
		servers_data.insert(std::make_pair(server_data->getSockFd(), server_data));
	}
	main_loop(servers_data, master_read, fd_max);
}

int main ()
{
	servero s1(8080);
	servero s2(8081);
	servero s3(8082);
	std::vector<servero> servers;
	servers.push_back(s1);
	// servers.push_back(s2);
	// servers.push_back(s3);

	// serving(servers);
	select_module(servers);
}