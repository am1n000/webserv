#include "../Includes/Client.hpp"
#include <stdexcept>
#include <string>
#include "../dev/dev.hpp"
#include "../Includes/Cgi.hpp"

Client::Client() {
	this->_changePtr = new struct kevent;
	this->req = new Request;
	this->resp = new Response(this->req);
}

Client::Client(int sockfd, bool listen_sock)
	: _sockFd(sockfd), _isListeningSock(listen_sock),
		_postFileCreated(0) {
	this->_changePtr = new struct kevent;
	this->req = new Request;
	this->resp = new Response(this->req);
}

Client::~Client() { 
  delete (this->_changePtr);
  delete this->req;
  delete this->resp;
}

void Client::prepareResponse() {
	this->resp->set_file(this->_sockFd);
}

bool Client::sending()
{
	bool finished = false;
	if (this->req->getRequestMethod() == GET)
	finished = this->resp->handle_get(this->_sockFd);
	else if (this->req->getRequestMethod() == POST)
	finished = this->resp->handle_post(this->_sockFd);
	else if (this->req->getRequestMethod() == DELETE)
	finished = this->resp->handle_delete(this->_sockFd);
	else
	throw std::runtime_error("method not found");
	return (finished);
}

bool Client::reading()
{
  // std::cout << "from reading method | : " << this->server->getRoot() << std::endl;
	char buffer[BUFFER_SIZE];
	int recieved_size;
	recieved_size = recv(this->_sockFd, buffer, BUFFER_SIZE, 0);
	if (recieved_size < 0)
		throw std::runtime_error("recv failed");
	this->req->appendBuffer(buffer, recieved_size);
	this->req->parseHeader();
	if (this->req->getRequestMethod() != POST)
	{
		this->prepareResponse();
		return (1);
	}
	if (this->req->isRequestCompleted())
		return (true);
	return (false);
}

//.getters
int Client::getSockFd() { return (this->_sockFd); }

bool Client::getIsListeningSock() { return (this->_isListeningSock); }

bool Client::getpostFileCreated() { return (this->_postFileCreated); }

struct kevent *Client::getChangePtr() { return (this->_changePtr); }


//.setters
void Client::setSockFd(int sockFd)
{
	this->_sockFd = sockFd;
	if (this->_sockFd == -2)
	this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
}

void Client::setIsListeningSock(int isListeningSock)
{
	this->_isListeningSock = isListeningSock;
}

void Client::setpostFileCreated(bool postFileCreated)
{
	this->_postFileCreated = postFileCreated;
}

void Client::setChangePtr()
{
	this->_changePtr = new struct kevent;
}
