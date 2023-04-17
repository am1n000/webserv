#include "../Includes/Client.hpp"
#include <stdexcept>
#include <string>
#include "../dev/dev.hpp"
#include "../Includes/Cgi.hpp"

Client::Client() {
  this->_changePtr = new struct kevent;
  this->req = new Request;
  this->resp = new Response;
}

Client::Client(int sockfd, int filtr, bool listen_sock)
    : _sockFd(sockfd), _filter(filtr), _isListeningSock(listen_sock),
      _postFileCreated(0) {
  this->_changePtr = new struct kevent;
  this->req = new Request;
  this->resp = new Response;
}

Client::~Client() { delete (this->_changePtr); }

void Client::prepareResponse() {
  this->resp->set_file(this->req->getFile(), this->_sockFd);
}

bool Client::sending(int kq) {
  bool finished = false;
  try {
    if (this->req->getRequestMethod() == GET)
      finished = this->resp->handle_get(this->_sockFd);
    else if (this->req->getRequestMethod() == POST)
      finished = this->resp->handle_post(this->_sockFd);
    else if (this->req->getRequestMethod() == DELETE)
      finished = this->resp->handle_delete(this->_sockFd);
    else
      throw std::runtime_error("method not found");
    // the exception trhown above is temporary
    // need to be handled lather on
  } catch (std::exception &e) {
    EV_SET(this->_changePtr, this->_sockFd, EVFILT_WRITE, EV_DELETE, 0, 0,
           this);
    kevent(kq, this->_changePtr, 1, NULL, 0, NULL);
    close(this->_sockFd);
  }
  if (finished) {

    EV_SET(this->_changePtr, this->_sockFd, EVFILT_WRITE, EV_DELETE, 0, 0,
           this);
    kevent(kq, this->_changePtr, 1, NULL, 0, NULL);
    close(this->_sockFd);
  }
  return (finished);
}

void Client::reading(int kq) {
  char buffer[BUFFER_SIZE];
  int recieved_size;
  try {
    recieved_size = recv(this->_sockFd, buffer, BUFFER_SIZE, 0);
    if (recieved_size < 0)
      throw std::runtime_error("recv failed");
    this->req->appendBuffer(buffer, recieved_size);
    if (this->req->isHeaderCompleted())
      this->req->parseHeader();
  } catch (std::exception &e) {
    EV_SET(this->_changePtr, this->_sockFd, EVFILT_READ, EV_DELETE, 0, 0, this);
    kevent(kq, this->_changePtr, 1, NULL, 0, NULL);
    close(this->_sockFd);
  }

  if (this->req->isRequestCompleted()) {
    /*
      * test cgi
      */
      if(this->req->getRequestMethod() == POST) {
      Cgi cgi(*this->req); 

      cgi.testCgi();
    }
      /*
      * test cgi end
      */
    this->prepareResponse();
    EV_SET(this->_changePtr, this->_sockFd, EVFILT_READ, EV_DELETE, 0, 0, this);
    if (kevent(kq, this->_changePtr, 1, NULL, 0, NULL) == -1)
      std::cerr << "error: kevent 3" << std::endl;
    EV_SET(this->_changePtr, this->_sockFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0,
           0, this);
    if (kevent(kq, this->_changePtr, 1, NULL, 0, NULL) == -1)
      std::cerr << "error: kevent 33" << std::endl;
    this->_filter = EVFILT_WRITE;
  }
}

//.getters
int Client::getSockFd() { return (this->_sockFd); }

int Client::getFilter() { return (this->_filter); }

int &Client::getHostAddrlen() { return (this->_hostAddrlen); }

bool Client::getIsListeningSock() { return (this->_isListeningSock); }

bool Client::getpostFileCreated() { return (this->_postFileCreated); }

struct kevent *Client::getChangePtr() { return (this->_changePtr); }

struct sockaddr_in &Client::getHostAddr() { return (this->_hostAddr); }

//.setters
void Client::setSockFd(int sockFd) {
  this->_sockFd = sockFd;
  if (this->_sockFd == -2)
    this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
}

void Client::setFilter(int filter) { this->_filter = filter; }

void Client::setHostAddrlen(int hostAddrlen) {
  this->_hostAddrlen = hostAddrlen;
}

void Client::setIsListeningSock(int isListeningSock) {
  this->_isListeningSock = isListeningSock;
}

void Client::setpostFileCreated(bool postFileCreated) {
  this->_postFileCreated = postFileCreated;
}

void Client::setChangePtr() { this->_changePtr = new struct kevent; }

void Client::setHostAddr(int port) {
  memset(&(this->_hostAddr), 0, sizeof(this->_hostAddr));
  this->_hostAddr.sin_family = AF_INET;   //? which protocol IPv4, IPv6, ...
  this->_hostAddr.sin_port = htons(port); //? the port in network byte order
  this->_hostAddr.sin_addr.s_addr =
      htonl(INADDR_ANY); //? the ip, use inet_addr function in case of and ip
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
// 	fcntl(this->_sockFd, F_SETFL, O_NONBLOCK); // if not the connection will
// block
// }

// void sock::bind_socket(int port)
// {
// 	this->host_addrlen = sizeof(this->host_addr);
// 	memset(&this->host_addr, 0, sizeof(this->host_addr));
// 	this->host_addr.sin_family = AF_INET;					//?
// which protocol IPv4, IPv6, ... 	this->host_addr.sin_port = htons(port);
// //? the port in network byte order 	this->host_addr.sin_addr.s_addr =
// htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
// 	// .allowing the socket to be reusable..................................
// 	int opt = 1;
// 	if (setsockopt(this->_sockFd, SOL_SOCKET, SO_REUSEADDR, &opt,
// sizeof(opt)) < 0)
// 	{
// 		std::cerr << "error: setsockopt" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// 	opt = 1;
// 	if (setsockopt(this->_sockFd, SOL_SOCKET, SO_NOSIGPIPE, &opt,
// sizeof(opt)) < 0)
// 	{
// 		std::cerr << "error: setsockopt" << std::endl;
// 		this->host_addr.sin_zero[0] = 'x';
// 	}
// 	// .binding the socket with an adress...................................
// 	if (bind(this->_sockFd, reinterpret_cast<struct sockaddr
// *>(&this->host_addr), this->host_addrlen) != 0) //? sockaddr is just an
// interface, is a sort of polymorphism
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
