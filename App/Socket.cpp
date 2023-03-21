#include "../Includes/Socket.hpp"

sock::sock()
{}

sock::~sock()
{}


void sock::prepare_response()
{
	this->resp.set_file(this->req.get_file());
}

void sock::sending()
{
	bool finished;
	try
	{
		finished = this->resp.upload(this->sock_fd);
	}
	catch (std::exception &e)
	{
		close(this->sock_fd);
	}
	if (finished)
		close (this->sock_fd);
}


void sock::reading(int kq, struct kevent *change)
{
	try
	{
		this->req.receive(kq, this, change);
	}
	catch(std::exception &e)
	{
		close(this->sock_fd);
	}
}

// sock::sock(sock const &obj)
// {}


// sock& sock::operator=(sock const &obj)
// {
// 	return (*this);
// }


// void sock::create_socket()
// {
// 	this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (this->sock_fd == -1)
// 	{
// 		std::cerr << "error: socket" << std::endl;
// 		return;
// 	}
// 	fcntl(this->sock_fd, F_SETFL, O_NONBLOCK); // if not the connection will block
// }

// void sock::bind_socket(int port)
// {
// 	this->host_addrlen = sizeof(this->host_addr);
// 	memset(&this->host_addr, 0, sizeof(this->host_addr));
// 	this->host_addr.sin_family = AF_INET;					//? which protocol IPv4, IPv6, ...
// 	this->host_addr.sin_port = htons(port);				//? the port in network byte order
// 	this->host_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
// 	// .allowing the socket to be reusable..................................
// 	int opt = 1;
// 	if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
// 	{
// 		std::cerr << "error: setsockopt" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// 	opt = 1;
// 	if (setsockopt(this->sock_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
// 	{
// 		std::cerr << "error: setsockopt" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// 	// .binding the socket with an adress...................................
// 	if (bind(this->sock_fd, reinterpret_cast<struct sockaddr *>(&this->host_addr), this->host_addrlen) != 0) //? sockaddr is just an interface, is a sort of polymorphism
// 	{
// 		std::cerr << "error: bind" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// }

// int sock::listen_to_connections()
// {
//     if (listen(this->sock_fd, SOMAXCONN) == -1)
//     {
//         std::cerr << "error: listen" << std::endl;
//         return (-1);
//     }
//     return (0); 
// }
