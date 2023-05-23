#include "../Includes/Client.hpp"
#include <stdexcept>
#include <string>
#include "../dev/dev.hpp"
#include "../Includes/Cgi.hpp"

Client::Client() {
	#ifdef __APPLE__
		this->_changePtr = new struct kevent;
	#endif
	this->req = new Request;
	this->resp = new Response(this->req);
}

Client::Client(int sockfd, bool listen_sock)
	: _sockFd(sockfd), _isListeningSock(listen_sock),
		_postFileCreated(0) {
	#ifdef __APPLE__
		this->_changePtr = new struct kevent;
	#endif
	this->req = new Request;
	this->resp = new Response(this->req);
}

Client::~Client()
{ 
	#ifdef __APPLE__
		delete (this->_changePtr);
	#endif
	delete this->req;
	delete this->resp;
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
	return (finished);
}

void	Client::checkServerByName()
{
	std::vector<Server*> &servers = Config::get()->getServers();
	std::string host = (this->req->getHeaders())["Host"];
	std::string domain = host.substr(0, host.find(":"));
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i]->getServerName() == domain)
		{
			this->server = Config::get()->getServers()[i];
    		this->req->setServer(this->server);
		}
	}
}

bool Client::reading()
{
	//! use .data field to initialize the buffer
	char buffer[2048];
	int recieved_size;
	recieved_size = recv(this->_sockFd, buffer, 2048, 0);
	if (recieved_size < 0)
		throw std::runtime_error("recv failed");
	this->req->appendBuffer(buffer, recieved_size);
	if (this->req->isRequestCompleted())
	{
		std::vector<std::string> indexes = this->server->getIndexes();
		checkServerByName();
		this->req->prepareRequest();
		return (true);
	}
	return (false);
}

//.getters
int Client::getSockFd() { return (this->_sockFd); }

bool Client::getIsListeningSock() { return (this->_isListeningSock); }

bool Client::getpostFileCreated() { return (this->_postFileCreated); }

#ifdef __APPLE__
	struct kevent *Client::getChangePtr() { return (this->_changePtr); }
#endif


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

#ifdef __APPLE__
	void Client::setChangePtr()
	{
		this->_changePtr = new struct kevent;
	}
#endif
