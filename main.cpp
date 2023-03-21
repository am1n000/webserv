#include "Includes/Socket.hpp"

int event_loop(struct kevent *change, int kq, sock *server_data, int connections)
{	
	while (1)
	{
		struct kevent events[MAX_EVENTS];
		int nevents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (nevents == -1)
			std::cerr << "error: kevent monitoring" << std::endl;
		for (int i = 0; i < nevents; i++)
		{
			sock *temp_data = (sock *)events[i].udata;
			if (temp_data->sock_fd == server_data->sock_fd)
			{
				sock *u_data = new sock;
				int client_sock = accept(server_data->sock_fd, reinterpret_cast<sockaddr *>(&(server_data->host_addr)), reinterpret_cast<socklen_t *>(&(server_data->host_addrlen)));
				if (client_sock == -1)
					std::cerr << "error: accept server " << std::endl;
				else
				{
					u_data->sock_fd = client_sock;
					u_data->id = connections;
					u_data->filter = EVFILT_READ;
					EV_SET(&change[connections], client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, u_data);
					if (kevent(kq, &change[connections], 1, NULL, 0, NULL) != 0)
						std::cout << "error: kevent registration 2" << std::endl;
					connections++;
				}
			}
			else
			{
				if (events[i].filter == EVFILT_READ)
					temp_data->reading(kq, change);
				else
					temp_data->sending();
			}
		}
	}

}

int main()
{
	struct kevent *change = new struct kevent[MAX_EVENTS];
	int kq = kqueue();
	if (kq == -1)
		std::cerr << "error: kqueue" << std::endl;
	sock *server_data = new sock;
	server_data->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_data->sock_fd == -1)
		std::cerr << "error: socket" << std::endl;
	fcntl(server_data->sock_fd, F_SETFL, O_NONBLOCK); // if not the connection will block
	server_data->host_addrlen = sizeof(server_data->host_addr);
	memset(&server_data->host_addr, 0, sizeof(server_data->host_addr));
	server_data->host_addr.sin_family = AF_INET;					//? which protocol IPv4, IPv6, ...
	server_data->host_addr.sin_port = htons(8080);				//? the port in network byte order
	server_data->host_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
	// .allowing the socket to be reusable..................................
	int opt = 1;
	if (setsockopt(server_data->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "error: setsockopt" << std::endl;
		server_data->host_addr.sin_zero[0] = -1;
	}
	opt = 1;
	if (setsockopt(server_data->sock_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "error: setsockopt" << std::endl;
		server_data->host_addr.sin_zero[0] = -1;
	}
	// .binding the socket with an adress...................................
	if (bind(server_data->sock_fd, reinterpret_cast<struct sockaddr *>(&server_data->host_addr), server_data->host_addrlen) != 0) //? sockaddr is just an interface, is a sort of polymorphism
	{
		std::cerr << "error: bind" << std::endl;
		server_data->host_addr.sin_zero[0] = -1;
	}
	if (listen(server_data->sock_fd, SOMAXCONN) == -1)
	{
		std::cerr << "error: listen" << std::endl;
		return (-1);
	}
	server_data->id = 0;
	EV_SET(&change[0], server_data->sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, server_data);
	if (kevent(kq, change, 1, NULL, 0, NULL) != 0)
		std::cout << "error: kevent registration 1" << std::endl;
	int connections = 1;
	event_loop(change, kq, server_data, connections);
}
