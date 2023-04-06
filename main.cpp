#include "Includes/Client.hpp"
#include "Includes/Client.hpp"
#include "Includes/ClientPoll.hpp"
#include "Includes/App.hpp"
#include "Includes/Config.hpp"
#include "Includes/Server.hpp"
#include "Includes/multiplexers/Kqueue.hpp"


//.kqueue.....................................................................................
int kqueue_event_loop(int kq)
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
			if (temp_data->getIsListeningSock())
			{
				int client_sock = accept(temp_data->getSockFd(),
					reinterpret_cast<sockaddr *>(&(temp_data->server->getHostAddr())),
					reinterpret_cast<socklen_t *>(&(temp_data->server->getHostAddrlen())));
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
			else
			{
				if (temp_data->getFilter() == EVFILT_READ)
				{
					try
					{
						if (temp_data->reading())
						{
							EV_SET(temp_data->getChangePtr(), temp_data->getSockFd(), EVFILT_READ,
									EV_DELETE, 0, 0, temp_data);
							if (kevent(kq, temp_data->getChangePtr(), 1, NULL, 0, NULL) == -1)
							std::cerr << "error: kevent 3" << std::endl;
							EV_SET(temp_data->getChangePtr(), temp_data->getSockFd(), EVFILT_WRITE,
									EV_ADD | EV_ENABLE,0,0, temp_data);
							if (kevent(kq, temp_data->getChangePtr(), 1, NULL, 0, NULL) == -1)
							std::cerr << "error: kevent 33" << std::endl;
							temp_data->setFilter(EVFILT_WRITE);
						}

					}
					catch (std::exception &e)
					{
						EV_SET(temp_data->getChangePtr(), temp_data->getSockFd(), EVFILT_READ, EV_DELETE, 0, 0, temp_data);
						kevent(kq, temp_data->getChangePtr(), 1, NULL, 0, NULL);
						close(temp_data->getSockFd());
					}
				}
				else
				{
					try
					{
						if (temp_data->sending())
						{
							EV_SET(temp_data->getChangePtr(), temp_data->getSockFd(), EVFILT_WRITE, EV_DELETE, 0, 0, temp_data);
							kevent(kq, temp_data->getChangePtr(), 1, NULL, 0, NULL);
							close(temp_data->getSockFd());
							delete (temp_data);
						}
					}
					catch (std::exception &e) //! to be modified according to every exception thrown
					{
						EV_SET(temp_data->getChangePtr(), temp_data->getSockFd(), EVFILT_WRITE, EV_DELETE, 0, 0, temp_data);
						kevent(kq, temp_data->getChangePtr(), 1, NULL, 0, NULL);
						close(temp_data->getSockFd());
					}
				}
				break;
			}
		}
	}

}
void kqueue_module()
{
  	std::vector<Server *> servers = Config::get()->getServers();
	int kq = kqueue();
	if (kq == -1)
		std::cerr << "error: kqueue" << std::endl;
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
	kqueue_event_loop(kq);
}
//............................................................................................

//.select.....................................................................................
void select_main_loop(std::vector<Client *> clients_data, fd_set read_master, int fd_max)
{
	fd_set write_master;
	FD_ZERO(&write_master);
	fd_set read_fds;
	fd_set write_fds;
	while (true)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		read_fds = read_master;
		write_fds = write_master;
		int ready_num = select(fd_max + 1, &read_fds, &write_fds, NULL, NULL);
		if (ready_num < 0)
		{
			std::cerr << "error : select  " << errno <<  std::endl;
			exit (1);
		}
		for (size_t i = 0; i < clients_data.size() || ready_num > 0; i++) //! decrement the ready_num each time 
		{
			if (FD_ISSET(clients_data[i]->getSockFd(), &read_fds))
			{
				if (clients_data[i]->getIsServerSock())
				{
					int client_sock = accept(clients_data[i]->getSockFd(), reinterpret_cast<sockaddr *>(&(clients_data[i]->getHostAddr())),
						reinterpret_cast<socklen_t *>(&(clients_data[i]->getHostAddrlen())));
					if (client_sock == -1)
						std::cerr << "error: accept server " << std::endl;
					else
					{
						Client *new_client = new Client(client_sock, 0);
						clients_data.push_back(new_client);
						FD_SET(client_sock, &read_master);
						if (client_sock > fd_max)
							fd_max = client_sock;
					}
				}
				else
				{
					try {
						clients_data[i]->reading(&read_master, &write_master);
					}
					catch(std::exception &e)
					{
						FD_CLR(clients_data[i]->getSockFd(), &read_master);
						close (clients_data[i]->getSockFd());
						clients_data.erase(clients_data.begin() + i);
					}
				}
			}
			else if (FD_ISSET(clients_data[i]->getSockFd(), &write_fds))
			{
				bool finito;
				try
				{
					finito = clients_data[i]->sending();
				}
				catch(std::exception &e)
				{
					FD_CLR(clients_data[i]->getSockFd(), &write_master);
					close (clients_data[i]->getSockFd());
					clients_data.erase(clients_data.begin() + i);
				}
				if (finito)
				{
					FD_CLR(clients_data[i]->getSockFd(), &write_master);
					close (clients_data[i]->getSockFd());
					clients_data.erase(clients_data.begin() + i);
				}
			}
			while (fd_max > 2)
			{
				if (FD_ISSET(fd_max, &read_master) || FD_ISSET(fd_max, &write_master))
					break;
				fd_max--;
			}
			ready_num--;
		}
	}
}
void select_module(std::vector<Server *> servers )
{
    fd_set master_read;
	FD_ZERO(&master_read);
	int	fd_max;
	std::vector<Client *> servers_data;
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
		FD_SET(server_data->getSockFd(), &master_read);
		fd_max = server_data->getSockFd();
		servers_data.push_back(server_data);
		fd_max = server_data->getSockFd();
	}
	select_main_loop(servers_data, master_read, fd_max);
}
// //............................................................................................

// //.poll.....................................................................................
// void poll_main_loop(std::vector<clientPoll *> clients_data, std::vector<struct pollfd> poll_fds)
// {
// 	while (true)
// 	{
// 		int ready_num = poll(&poll_fds[0], poll_fds.size() , -1);
// 		if (ready_num < 0)
// 		{
// 			std::cerr << "error : poll  " <<  std::endl;
// 			exit (1);
// 		}
// 		// std::cout << ready_num << std::endl;
// 		for (size_t i = 0; i < poll_fds.size() || ready_num > 0; i++) //! decrement the ready_num each time 
// 		{
// 			if (poll_fds[i].revents & POLLIN)
// 			{
// 				if (clients_data[i]->getIsServerSock())
// 				{
// 					int client_sock = accept(clients_data[i]->getSockFd(), reinterpret_cast<sockaddr *>(&(clients_data[i]->getHostAddr())),
// 						reinterpret_cast<socklen_t *>(&(clients_data[i]->getHostAddrlen())));
// 					if (client_sock == -1)
// 						std::cerr << "error: accept server " << std::endl;
// 					else
// 					{
// 						clientPoll *new_client = new clientPoll(client_sock, 0);
// 						clients_data.push_back(new_client);
// 						struct pollfd pfd;
// 						pfd.fd = client_sock;
// 						pfd.events = POLLIN;
// 						poll_fds.push_back(pfd);
// 					}
// 				}
// 				else
// 				{
// 					try {
// 						clients_data[i]->reading(&poll_fds, i);
// 					}
// 					catch(std::exception &e)
// 					{
// 						close (clients_data[i]->getSockFd());
// 						clients_data.erase(clients_data.begin() + i);
// 						poll_fds.erase(poll_fds.begin() + i);
// 					}
// 				}
// 			}
// 			else if (poll_fds[i].revents & POLLOUT)
// 			{
// 				bool finito;
// 				try
// 				{
// 					finito = clients_data[i]->sending();
// 				}
// 				catch(std::exception &e)
// 				{
// 					close (clients_data[i]->getSockFd());
// 					clients_data.erase(clients_data.begin() + i);
// 					poll_fds.erase(poll_fds.begin() + i);
// 				}
// 				if (finito)
// 				{
// 					close (clients_data[i]->getSockFd());
// 					clients_data.erase(clients_data.begin() + i);
// 					poll_fds.erase(poll_fds.begin() + i);
// 				}
// 			}
// 			ready_num--;
// 		}
// 	}

// }
// void poll_module(std::vector<servero> servers)
// {
// 	std::vector<clientPoll *> servers_data;
// 	std::vector<struct pollfd> poll_fds;
//     for (size_t j = 0; j < servers.size(); j++)
// 	{
// 		clientPoll *server_data = new clientPoll;
// 		struct pollfd	pfd;
// 		server_data->setSockFd(-2);
// 		if (server_data->getSockFd() == -1)
// 		{
// 			std::cerr << "error: socket" << std::endl;
// 			continue;
// 		}
// 		fcntl(server_data->getSockFd(), F_SETFL, O_NONBLOCK); // if not the connection will block
// 		server_data->setHostAddrlen(sizeof(server_data->getHostAddr()));
// 		server_data->setHostAddr(servers[j]._port);
// 		// .allowing the socket to be reusable..................................
// 		int opt = 1;
// 		if (setsockopt(server_data->getSockFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
// 		{
// 			std::cerr << "error: setsockopt" << std::endl;
// 			continue;
// 		}
// 		opt = 1;
// 		if (setsockopt(server_data->getSockFd(), SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
// 		{
// 			std::cerr << "error: setsockopt" << std::endl;
// 			continue;
// 		}
// 		// .binding the socket with an adress...................................
// 		if (bind(server_data->getSockFd(), reinterpret_cast<struct sockaddr *>(&(server_data->getHostAddr())),
// 			server_data->getHostAddrlen()) != 0) //? sockaddr is just an interface, is a sort of polymorphism
// 		{
// 			std::cerr << "error: bind" << std::endl;
// 			continue;
// 		}
// 		if (listen(server_data->getSockFd(), SOMAXCONN) == -1)
// 		{
// 			std::cerr << "error: listen" << std::endl;
// 			continue;
// 		}
// 		server_data->setIsServerSock(1);
// 		servers_data.push_back(server_data);
// 		pfd.fd = server_data->getSockFd();
// 		pfd.events = POLLIN;
// 		poll_fds.push_back(pfd);
// 	}
// 	poll_main_loop(servers_data, poll_fds);
// }
//..........................................................................................

//!change map to vector for both select and poll clients
int main ()
{
	app();
  	std::vector<Server *> servers = Config::get()->getServers();
	// Kqueue::serve(servers);
	select_module(servers);
	// poll_module(servers);
}     