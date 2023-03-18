#include "Includes/Socket.hpp"


int preparation(int kq, std::vector<server> servero, struct kevent *change, struct kevent *events)
{
	int connections = 0;

	for (int i = 0; i < servero.size(); i++)
	{
		sock *serv = new sock;
		serv->fails = 0;
		serv->create_socket();
		if (serv->sock_fd == -1)
			serv->fails = 1;
		else
			std::cout << "socket n:  " << i << " created successfully!" << std::endl;
		serv->bind_socket(servero[i].port);
		if (serv->host_addr.sin_zero[0] == 'x')
			serv->fails = 0;
		else
			std::cout << "socket n:  " << i << " bounded successfully!" << std::endl;
		serv->host_addrlen = sizeof(serv->host_addr);
		serv->id = i + 1;
		serv->port = servero[i].port;
		serv->is_listen_socket = 1;
		if (serv->listen_to_connections() == -1)
			serv->fails = 1;
		else
			std::cout << "server n:  " << i << " listening  for  connections!" << std::endl;
		EV_SET(&change[i], serv->sock_fd, EVFILT_READ, EV_ADD, 0, 0, serv); 
		if (kevent(kq, &change[i], 1, NULL, 0, NULL) == -1) //  kevent registation
		{
			std::cerr << "error: kevent registration" << std::endl;
			serv->fails = 1;
		}
		if (serv->fails == 0)
			connections++;
	}
	return (connections);
}

void reading(int kq, struct kevent *change, struct kevent *events, int i, sock *u_data)
{
	char buff[BUFFER_SIZE];
	std::cout << "ok ok ok" << std::endl;
	int bytes_read = recv(u_data->sock_fd, buff, BUFFER_SIZE, 0);
	if (bytes_read < 0)
	{
		std::cerr << "error: recv" << std::endl;
		close (u_data->sock_fd);
		EV_SET(change, u_data->sock_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		kevent(kq, change, 1, NULL, 0, NULL); // remove the socket from the change
		std::cout << " 1 1 1 " <<std::endl;
		return ;
	}
	else if (bytes_read == 0 || strstr(buff, "\r\n\r\n"))
	{
		// ? change the event from the read filter to the write filter
		u_data->reading_buffer += strtok(buff, "\r\n\r\n");
		std::cout << "ok2 ok2 ok2" << std::endl;
		EV_SET(change,u_data->sock_fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, u_data);
		if (kevent(kq, change, 1, NULL, 0, NULL) == -1)
			std::cerr << "error: kevent registration 3" << std::endl;
		char *temp = strdup((u_data->reading_buffer).c_str());
		u_data->req.parse_request_line(strtok(temp, "\n"));
		u_data->finished_reading_header = 1;
		// std::cout << "--------------------------------" << std::endl;
		// std::cout << u_data->reading_buffer << std::endl; // printing the request
		// std::cout << "--------------------------------" << std::endl;
	}
	else
	{
		u_data->reading_buffer += buff;
	}
}

void writing(int kq, sock *u_data, struct kevent *change)
{	
	// Open the requested file
	s_file ressource = u_data->req.get_file();
	// if (u_data->oppened_file == 0)
	// {
		std::ifstream file((ressource.filename).c_str(), std::ios::binary);
		// if (!file.is_open())
		// {
		// 	std::cerr << "error: file" << std::endl;
		// 	close (u_data->sock_fd);
		// 	EV_SET(change, u_data->sock_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		// 	kevent(kq, change, 1, NULL, 0, NULL); // remove the socket from the change
		// std::cout << " 2 2 2 2 " <<std::endl;
			// return;
		// }

		// Send the HTTP response header
		std::string resp = "HTTP/1.0 200 OK\r\n"
						"Server: webserver-c\r\n"
						"Content-type: ";
		resp += ressource.media;
		resp += "\r\n\r\n";
		int k = send(u_data->sock_fd, resp.c_str(), resp.length(), 0);
		if (k == -1)
		{
			std::cerr << "error: sending 1" << std::endl;
			return;
		}
		// file.seekg(0, std::ios::end);
		// u_data->bytes_to_send = file.tellg();
		// file.seekg(0, std::ios::beg);
		// u_data->writing_buffer = new char[u_data->bytes_to_send];
		// file.read(u_data->writing_buffer, u_data->bytes_to_send);
		// u_data->oppened_file = 1;
		// return;
	// }
	// 	int len = 1024;
	// 	if(u_data->bytes_sent + len > u_data->bytes_to_send)
	// 		len = u_data->bytes_to_send - u_data->bytes_sent;
	// 	int k = send(u_data->sock_fd, u_data->writing_buffer + u_data->bytes_sent, len, 0);
	// 	if (k == -1)
	// 	{
	// 		std::cerr << "error: sending 2" << std::endl;
	// 		return;
	// 	}
	// 	u_data->bytes_sent += len;
	// 	if (u_data->bytes_sent >= u_data->bytes_to_send)
	// 	{
			close (u_data->sock_fd);
			EV_SET(change, u_data->sock_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
			kevent(kq, change, 1, NULL, 0, NULL); // remove the socket from the change
			std::cout << "3 3 3 3 " << std::endl;
			file.close();
			return ;
	// 	}
}


void event_loop(int kq, struct kevent *change, struct kevent *events, int connections)
{
	while (true)
	{
		int nevents;

		//. waiting for events........................................
		nevents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		std::cout << "number of events:      " << nevents << std::endl;
		if (nevents == -1)
		{
			std::cerr << "error: kevent waiting events" << std::endl;
			return;
		}
		//. handing events...............................................
		for (int i = 0; i < nevents; i++)
		{
			int event_sock = events[i].ident;
			sock *u_data = (sock *)events[i].udata;

			if (u_data->is_listen_socket)
			{
				std::cout << "1111111111111" << std::endl;
				// .Accepting connections...........................................
				int client_sock_fd = accept(u_data->sock_fd, reinterpret_cast<struct sockaddr *>(&u_data->host_addr), reinterpret_cast<socklen_t *>(&u_data->host_addrlen));
				if (client_sock_fd == -1)
					std::cerr << "error: accept (server n: " << u_data->id << ")" << std::endl;
				else
				{
					//.adding socket to queue
					sock *new_socket = new sock;
					new_socket->sock_fd = client_sock_fd;
					new_socket->id = connections + 1;
					new_socket->is_listen_socket = 0;
					new_socket->finished_reading_header = 0;
					new_socket->oppened_file = 0;
					EV_SET(&change[connections], new_socket->sock_fd, EVFILT_READ, EV_ADD, 0, 0, new_socket);
					if (kevent(kq, &change[connections], 1, NULL, 0, NULL) == -1)
						std::cerr << "error: kevent registration 2" << std::endl;
					connections++;
				}
			}
			else
			{
				if (events[i].filter == EVFILT_READ)
				{

				std::cout << "222222222222" << std::endl;
					reading(kq, change, events, i , u_data);
				}
				else
				{
				std::cout << "333333333" << std::endl;

					writing(kq, u_data ,change);
				}
			}
		}

	}
}

int serving(std::vector<server> servero)
{
	int kq = kqueue(); //? creating new queue
	struct kevent *change;
	struct kevent *events;

	change = new struct kevent[MAX_EVENTS];
	events = new struct kevent[MAX_EVENTS];
	if (kq == -1)
	{
		std::cerr << "error: kqueue" << std::endl;
		return (1);
	}
    int connections = preparation(kq, servero, change, events);
	event_loop(kq, change, events, connections);
    return (0);
}

int main ()
{
	server s1(8080);
	server s2(8081);
	server s3(8082);
	std::vector<server> servero;
	servero.push_back(s1);
	servero.push_back(s2);
	servero.push_back(s3);

	serving(servero);
}