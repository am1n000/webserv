#include "Includes/Socket.hpp"
#include <ctime>
#include <string.h>
#include <unistd.h>

typedef struct t_data
{
	char buffer[2048];
	int sock_fd;
	struct sockaddr_in host_addr;
	int host_addrlen;
	request req;
	int bytes_to_send;
	char *writing_buffer;
	int oppened_file;
	int bytes_sent;
	int id;
	int filter;
	std::ifstream file;
}				s_data;

char *my_tostring(int num)
{
    int i = 0, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
	if  (len == 1)
		len = 2;
	char *str = new char[len];
	if  (len == 1)
	{
		str[0] = '0';
		i = 1;
	}
    for (;i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
	return (str);
}
std::string get_time()
{
	time_t now = time(0);

	tm* local_time = localtime(&now);

	int year = local_time->tm_year + 1900;
	int day = local_time->tm_mday;
	int hour = local_time->tm_hour;
	int min = local_time->tm_min;
	int sec = local_time->tm_sec;

	char month_name[4];
	char day_name[4];
	strftime(day_name, 4, "%a", local_time);
	strftime(month_name, 4, "%b", local_time);

	std::string now_time;
	now_time = day_name;
	now_time += ", ";
	now_time += my_tostring(day);
	now_time += " ";
	now_time += month_name;
	now_time += " ";
	now_time += my_tostring(year);
	now_time += " ";
	now_time += my_tostring(hour);
	now_time += ":";
	now_time += my_tostring(min);
	now_time += ":";
	now_time += my_tostring(sec);
	now_time += " ";
	now_time += "GMT";
	return (now_time);
}

void serve_file(s_data *u_data)
{	
	// Open the requested file
	s_file ressource = u_data->req.get_file();
	if (u_data->oppened_file == 0)
	{
		u_data->file.open((ressource.filename).c_str(), std::ios::binary);
		if (!u_data->file.is_open())
		{
			std::cerr << "error: file" << std::endl;
			close (u_data->sock_fd);
			return;
		}

		// Send the HTTP response header
		std::string resp = "HTTP/1.1 200 OK\r\n"
						"Server: webserver-c\r\n"
						"Content-type: ";
		resp += ressource.media;
		resp += "\r\n";
		resp += "Last-Modified: ";
		resp += get_time();
		resp += "\r\n\r\n";
		int k = send(u_data->sock_fd, resp.c_str(), resp.length(), 0);
		if (k == -1)
		{
			std::cerr << "error: sending 1" << std::endl;
			return;
		}
		u_data->file.seekg(0, std::ios::end);
		u_data->bytes_to_send = u_data->file.tellg();
		u_data->file.seekg(0, std::ios::beg);
		u_data->oppened_file = 1;
		return;
	}
		int len = 1000;
		if(u_data->bytes_sent + len >= u_data->bytes_to_send)
			len = u_data->bytes_to_send - u_data->bytes_sent;
		char buffer[len];
		u_data->file.read(buffer, len);
		int k = send(u_data->sock_fd, buffer, len, 0);
		if (k == -1)
		{
			close (u_data->sock_fd);
			return;
		}
		u_data->bytes_sent += len;
		if (u_data->bytes_sent >= u_data->bytes_to_send)
		{
			close (u_data->sock_fd);
			u_data->file.close();
			return ;
		}
}

int event_loop(struct kevent *change, int kq, s_data *server_data, int connections)
{	
	while (1)
	{
		struct kevent events[MAX_EVENTS];
		int nevents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (nevents == -1)
			std::cerr << "error: kevent monitoring" << std::endl;
		for (int i = 0; i < nevents; i++)
		{
			s_data *temp_data = (s_data *)events[i].udata;
			if (temp_data->sock_fd == server_data->sock_fd)
			{
				s_data *u_data = new s_data;
				int client_sock = accept(server_data->sock_fd, reinterpret_cast<sockaddr *>(&(server_data->host_addr)), reinterpret_cast<socklen_t *>(&(server_data->host_addrlen)));
				if (client_sock == -1)
					std::cerr << "error: accept server " << std::endl;
				else
				{
					u_data->sock_fd = client_sock;
					u_data->id = connections;
					u_data->bytes_sent = 0;
					u_data->oppened_file = 0;
					u_data->writing_buffer = nullptr;
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
				{
					int val_read = recv(temp_data->sock_fd, temp_data->buffer, BUFFER_SIZE, 0);
					if (val_read < 0)
					{
						if (val_read < 0)
							std::cerr << "error: recv" << std::endl;
						close(temp_data->sock_fd);
						continue;
					}
					if (val_read == 0 || strstr(temp_data->buffer, "\r\n"))
					{
						EV_SET(&change[temp_data->id], temp_data->sock_fd, EVFILT_READ, EV_DELETE, 0, 0, temp_data);
						if (kevent(kq, &change[temp_data->id], 1, NULL, 0, NULL) == -1)
							std::cerr << "error: kevent 3" << std::endl;
						EV_SET(&change[temp_data->id], temp_data->sock_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, temp_data);
						if (kevent(kq, &change[temp_data->id], 1, NULL, 0, NULL) == -1)
							std::cerr << "error: kevent 33" << std::endl;
						temp_data->filter = EVFILT_WRITE;
						temp_data->req.parse_request_line(strtok(temp_data->buffer, "\r\n"));
					}

					// . printing the request...........................................
				}
				else
					serve_file(temp_data);
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
	s_data *server_data = new s_data;
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