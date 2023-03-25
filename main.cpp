#include "Includes/Socket.hpp"


class servero
{
	public:
		int _port;
		servero(int port) : _port(port) {};
		~servero() {};
};

int event_loop(int kq, std::vector<sock*> servers_data)
{
	while (1)
	{
		struct kevent events[MAX_EVENTS];
		int nevents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (nevents == -1)
			std::cerr << "error: kevent monitoring" << std::endl;
		for (int j = 0; j < nevents; j++)
		{
			sock *temp_data = (sock *)events[j].udata;
			for (int i = 0; i < servers_data.size(); i++)
			{
				if (temp_data->sock_fd == servers_data[i]->sock_fd)
				{
					int client_sock = accept(servers_data[i]->sock_fd, reinterpret_cast<sockaddr *>(&(servers_data[i]->host_addr)), reinterpret_cast<socklen_t *>(&(servers_data[i]->host_addrlen)));
					if (client_sock == -1)
						std::cerr << "error: accept server " << std::endl;
					else
					{
						sock *u_data = new sock(client_sock, EVFILT_READ, 0);
						EV_SET(u_data->change_ptr, client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, u_data);
						if (kevent(kq, u_data->change_ptr, 1, NULL, 0, NULL) != 0)
							std::cerr << "error: kevent registration 2" << std::endl;
					}
					break;
				}
				else if (!temp_data->is_listening_sock)
				{
					if (temp_data->filter == EVFILT_READ)
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
	std::vector<sock *> servers_data;
	if (kq == -1)
		std::cerr << "error: kqueue" << std::endl;
	for (int j = 0; j < servers.size(); j++)
	{
		sock *server_data = new sock;
		int valid = 1;
		server_data->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_data->sock_fd == -1)
		{
			std::cerr << "error: socket" << std::endl;
			continue;
		}
		fcntl(server_data->sock_fd, F_SETFL, O_NONBLOCK); // if not the connection will block
		server_data->host_addrlen = sizeof(server_data->host_addr);
		memset(&(server_data->host_addr), 0, sizeof(server_data->host_addr));
		server_data->host_addr.sin_family = AF_INET;					//? which protocol IPv4, IPv6, ...
		server_data->host_addr.sin_port = htons(servers[j]._port);				//? the port in network byte order
		server_data->host_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
		// .allowing the socket to be reusable..................................
		int opt = 1;
		if (setsockopt(server_data->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "error: setsockopt" << std::endl;
			continue;
		}
		opt = 1;
		if (setsockopt(server_data->sock_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "error: setsockopt" << std::endl;
			continue;
		}
		// .binding the socket with an adress...................................
		if (bind(server_data->sock_fd, reinterpret_cast<struct sockaddr *>(&(server_data->host_addr)), server_data->host_addrlen) != 0) //? sockaddr is just an interface, is a sort of polymorphism
		{
			std::cerr << "error: bind" << std::endl;
			continue;
		}
		if (listen(server_data->sock_fd, SOMAXCONN) == -1)
		{
			std::cerr << "error: listen" << std::endl;
			continue;
		}
		// server_data->change_ptr = new struct kevent;
		EV_SET(server_data->change_ptr, server_data->sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, server_data);
		if (kevent(kq, server_data->change_ptr, 1, NULL, 0, NULL) != 0)
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