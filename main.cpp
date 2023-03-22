#include "Includes/Socket.hpp"


class servero
{
	public:
		int _port;
		servero(int port) : _port(port) {};
		~servero() {};
};

int event_loop(struct kevent *change, int kq, sock *server_data, int connections)
{
	int servers_count = connections;
	while (1)
	{
		struct kevent events[MAX_EVENTS];
		int nevents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (nevents == -1)
			std::cerr << "error: kevent monitoring" << std::endl;
		for (int i = 0; i < nevents; i++)
		{
			sock *temp_data = (sock *)events[i].udata;
			for (int i = 0; i < servers_count; i++)
			{
				if (temp_data->sock_fd == server_data[i].sock_fd)
				{
					sock *u_data = new sock;
					int client_sock = accept(server_data[i].sock_fd, reinterpret_cast<sockaddr *>(&(server_data[i].host_addr)), reinterpret_cast<socklen_t *>(&(server_data[i].host_addrlen)));
					if (client_sock == -1)
						std::cerr << "error: accept server " << std::endl;
					else
					{
						u_data->sock_fd = client_sock;
						u_data->id = connections;
						u_data->is_listening_sock = 0;
						u_data->filter = EVFILT_READ;
						EV_SET(&change[connections], client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, u_data);
						if (kevent(kq, &change[connections], 1, NULL, 0, NULL) != 0)
							std::cerr << "error: kevent registration 2" << std::endl;
						connections++;
					}
					break;
				}
				else if (!temp_data->is_listening_sock)
				{
					if (temp_data->filter == EVFILT_READ)
						temp_data->reading(kq, change);
					else
						temp_data->sending(kq, change);
					break;
				}
			}
		}
	}

}

void serving(std::vector<servero> servers)
{
	struct kevent *change = new struct kevent[MAX_EVENTS];
	int connections = 1;

	int kq = kqueue();
	sock *server_data = new sock[servers.size()];
	if (kq == -1)
		std::cerr << "error: kqueue" << std::endl;
	int i = 0;
	for (int j = 0; j < servers.size(); j++)
	{
		int valid = 1;
		server_data[i].sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_data[i].sock_fd == -1)
		{
			std::cerr << "error: socket" << std::endl;
			continue;
		}
		fcntl(server_data[i].sock_fd, F_SETFL, O_NONBLOCK); // if not the connection will block
		server_data[i].host_addrlen = sizeof(server_data[i].host_addr);
		memset(&(server_data[i].host_addr), 0, sizeof(server_data[i].host_addr));
		server_data[i].host_addr.sin_family = AF_INET;					//? which protocol IPv4, IPv6, ...
		server_data[i].host_addr.sin_port = htons(servers[j]._port);				//? the port in network byte order
		server_data[i].host_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
		// .allowing the socket to be reusable..................................
		int opt = 1;
		if (setsockopt(server_data[i].sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "error: setsockopt" << std::endl;
			continue;
		}
		opt = 1;
		if (setsockopt(server_data[i].sock_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "error: setsockopt" << std::endl;
			continue;
		}
		// .binding the socket with an adress...................................
		if (bind(server_data[i].sock_fd, reinterpret_cast<struct sockaddr *>(&(server_data[i].host_addr)), server_data[i].host_addrlen) != 0) //? sockaddr is just an interface, is a sort of polymorphism
		{
			std::cerr << "error: bind" << std::endl;
			continue;
		}
		if (listen(server_data[i].sock_fd, SOMAXCONN) == -1)
		{
			std::cerr << "error: listen" << std::endl;
			continue;
		}
		server_data[i].id = 0;
		server_data[i].is_listening_sock = 1;
		EV_SET(&change[i], server_data[i].sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, &server_data[i]);
		if (kevent(kq, &change[i], 1, NULL, 0, NULL) != 0)
		{
			std::cerr << "error: kevent registration 1" << std::endl;
			continue;
		}
		i++;
	}
	event_loop(change, kq, server_data, i);
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