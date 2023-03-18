#ifndef SOCKET_HPP

#define SOCKET_HPP


#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <fstream>
#include <map>
#include <vector>
#include <poll.h>
#include <sys/event.h>

#define BUFFER_SIZE 100
#define MAX_EVENTS 10

class request
{
	private:
		int _method; // ? 1: GET, 2: POST, 3: DELETE
		s_file _file;
		int _version;

	
	public:
		request();
		~request();
		request (const request &x);
		request &operator= (const request &x);
		int             get_method() const;
		s_file			get_file() const;
		int             get_version() const;
		void			set_file(char *file);
		void            parse_request_line(char *line);
		std::string 	media_type(std::string extension);
};

class server
{
	public:
		int listening_socket;
		struct sockaddr_in host_addr;
		int host_addrlen;
		int port;
		bool fails;
		server(int p) : port(p) {};
		~server() {};

};

class the_data
{
	public:
		int id;
		int sock_fd;
		int file_fd;
		int to_be_read;
		int to_be_written;
		int b_read;
		int b_sent;
		int a;
		int port;
		bool is_listen_socket;
		struct sockaddr_in host_addr;
		int host_addrlen;
		std::string buffer;
		request	req;
};

class sock
{
	public:
		sock();
		~sock();
		sock(sock const &obj);
		sock& operator=(sock const &obj);
		
		// int serving(std::vector<server> servero);
		void create_socket();
		void bind_socket(int port);
		int listen_to_connections();
		// std::vector<sock> *preparation(int kq, std::vector<server> servero, struct kevent *change, struct kevent *events);
		// void event_loop(int kq, struct kevent *change, struct kevent *events, std::vector<sock> *sockets);
		// void reading(int kq, struct kevent *change, struct kevent *events, int i, sock *u_data);
		// void writing(sock *u_data);

		//variables
		int id;
		int sock_fd;
		int file_fd;
		int fails;
		int port;
		bool is_listen_socket;
		struct sockaddr_in host_addr;
		int host_addrlen;
		std::string buffer;
		request	req;
		bool finished_reading_header;
		int finished_writing;
		s_file ressource;
		
};


#endif