#include "../Includes/ClientPoll.hpp"

clientPoll::clientPoll()
{
	this->req = new Request;
	this->resp = new Response;
}

clientPoll::clientPoll(int sockfd, bool listen_sock) : _sockFd(sockfd), _isServerSock(listen_sock)
{
	this->req = new Request;
	this->resp = new Response;
}

clientPoll::~clientPoll()
{}


void clientPoll::prepare_response()
{
	this->resp->set_file(this->req->get_file(), this->_sockFd);
}


bool clientPoll::sending()
{
	bool finished = false;
	if (this->req->get_method() == 1)
		finished = this->resp->handle_get(this->_sockFd);
	else if (this->req->get_method() == 2)
		finished = this->resp->handle_post(this->_sockFd);
	else
		finished = this->resp->handle_delete(this->_sockFd);
	return (finished);
}


void clientPoll::reading(std::vector<struct pollfd> *poll_fds, int pos)
{
	char buffer[BUFFER_SIZE + 1];
	int val_read;
	char *temp_buff;
	val_read = recv(this->_sockFd, buffer, BUFFER_SIZE, 0);
	if (val_read < 0)
	{
		throw(RecvFailedException());
	}
	buffer[val_read] = '\0';
	temp_buff = strdup(buffer);
	this->req->reading_buffer += buffer;
	if (this->req->content_lenght == 0 && (val_read == 0 || std::strstr(buffer, "\r\n\r\n")))
	{
		this->req->parse_request_line(strtok(buffer, "\r\n\r\n"));
		if (this->req->get_method() != 2)
			this->prepare_response();
		// this->req->content_lenght = 135154;
	}
	if(this->req->get_method() == 2)
	{
		if (this->_postFileCreated == 0)
		{
			this->req->post_file.open("ressources/post/test2.txt");
			if (!this->req->post_file.is_open())
			{
				std::cerr << "post file open error" << std::endl;
				throw(FileNotFound());
			}
			char *bod = std::strstr(temp_buff, "\r\n\r\n");
			this->req->post_file.write(bod, std::strlen(bod));
			this->req->content_lenght -= std::strlen(bod);
			this->_postFileCreated = 1;
		}
		else
		{
			this->req->post_file.write(temp_buff, val_read);
			this->req->content_lenght -= val_read;
		}
	}
	if (this->req->content_lenght <= 0)
	{
		(*poll_fds)[pos].events = POLLOUT;
	}
}



//.getters
int	clientPoll::getSockFd()
{
	return (this->_sockFd);	
}


int&	clientPoll::getHostAddrlen()
{
	return (this->_hostAddrlen);	
}

bool	clientPoll::getIsServerSock()
{
	return (this->_isServerSock);	
}

struct sockaddr_in&	clientPoll::getHostAddr()
{
	return (this->_hostAddr);	
}

bool	clientPoll::getpostFileCreated()
{
	return (this->_postFileCreated);	
}


//.setters
void	clientPoll::setSockFd(int sockFd)
{
	this->_sockFd = sockFd;	
	if (this->_sockFd == -2)
		this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);		
}

void	clientPoll::setHostAddrlen(int hostAddrlen)
{
	this->_hostAddrlen = hostAddrlen;	
}

void	clientPoll::setIsServerSock(int isServerSock)
{
	this->_isServerSock = isServerSock;	
}

void	clientPoll::setpostFileCreated(bool postFileCreated)
{
	this->_postFileCreated = postFileCreated;	
}

void	clientPoll::setHostAddr(int port)
{
	memset(&(this->_hostAddr), 0, sizeof(this->_hostAddr));
	this->_hostAddr.sin_family = AF_INET;					//? which protocol IPv4, IPv6, ...
	this->_hostAddr.sin_port = htons(port);				//? the port in network byte order
	this->_hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
}









// sock::sock(sock const &obj)
// {}


// sock& sock::operator=(sock const &obj)
// {
// 	return (*this);
// }


// void sock::create_socket()
// {
// 	this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (this->_sockFd == -1)
// 	{
// 		std::cerr << "error: socket" << std::endl;
// 		return;
// 	}
// 	fcntl(this->_sockFd, F_SETFL, O_NONBLOCK); // if not the connection will block
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
// 	if (setsockopt(this->_sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
// 	{
// 		std::cerr << "error: setsockopt" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// 	opt = 1;
// 	if (setsockopt(this->_sockFd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
// 	{
// 		std::cerr << "error: setsockopt" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// 	// .binding the socket with an adress...................................
// 	if (bind(this->_sockFd, reinterpret_cast<struct sockaddr *>(&this->host_addr), this->host_addrlen) != 0) //? sockaddr is just an interface, is a sort of polymorphism
// 	{
// 		std::cerr << "error: bind" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// }

// int sock::listen_to_connections()
// {
//     if (listen(this->_sockFd, SOMAXCONN) == -1)
//     {
//         std::cerr << "error: listen" << std::endl;
//         return (-1);
//     }
//     return (0); 
// }
