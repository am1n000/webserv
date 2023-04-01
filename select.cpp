#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include "Includes/ClientSelect.hpp"

class servero
{
	public:
		int _port;
		servero(int port) : _port(port) {};
		~servero() {};
};

void main_loop(std::map<int, clientSelect *> servers_data, fd_set read_master, int fd_max)
{
	fd_set write_master;
	while (true)
	{
		fd_set read_fds = read_master;
		fd_set write_fds = write_master;
		int ready_num = select(fd_max + 1, &read_fds, &write_fds, NULL, NULL);
		if (ready_num < 0)
		{
			std::cerr << "error : select" << std::endl;
			exit (1);
		}
		for (int i = 3; i <= fd_max; i++)
		{
			std::map<int, clientSelect *>::iterator it = servers_data.find(i);
			if (FD_ISSET(i, &read_fds))
			{
				if (it != servers_data.end())
				{
					int client_sock = accept(it->second->getSockFd(), reinterpret_cast<sockaddr *>(&(it->second->getHostAddr())),
						reinterpret_cast<socklen_t *>(&(it->second->getHostAddrlen())));
					if (client_sock == -1)
						std::cerr << "error: accept server " << std::endl;
					else
						FD_SET(client_sock, &read_master);
				}
				else
					it->second->reading(&read_master, &write_master);
			}
			else
				it->second->sending(&write_master);
		}
	
	}

}

void select_module(std::vector<servero> servers)
{
    fd_set master;
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
		FD_SET(server_data->getSockFd(), &master);
		fd_max = server_data->getSockFd();
		servers_data.insert(std::make_pair(server_data->getSockFd(), server_data));
	}
	main_loop(servers_data, master, fd_max);
}