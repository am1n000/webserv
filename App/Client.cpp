#include "../Includes/Client.hpp"
#include <stdexcept>
#include <string>
#include "../dev/dev.hpp"
#include "../Includes/Cgi.hpp"

Client::Client() {
	#ifdef __APPLE__
		this->_changePtr = new struct kevent;
		this->_timeoutChangePtr = new struct kevent;
	#endif
	this->req = new Request;
	this->resp = new Response(this->req);
}

Client::Client(int sockfd, bool listen_sock)
	: _sockFd(sockfd), _isListeningSock(listen_sock),
		_postFileCreated(0) {
	#ifdef __APPLE__
		this->_changePtr = new struct kevent;
		this->_timeoutChangePtr = new struct kevent;
	#endif
	this->req = new Request;
	this->resp = new Response(this->req);
}

Client::~Client()
{ 
	#ifdef __APPLE__
		delete (this->_changePtr);
		delete (this->_timeoutChangePtr);
	#endif
	delete this->req;
	delete this->resp;
}

bool Client::sending()
{
  return this->resp->respond(this->_sockFd);
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
	if (recieved_size  == -1)
		throw (InternalServerErrorException());
	else if (recieved_size == 0)
		return (1);
	this->req->appendBuffer(buffer, recieved_size);
	if (this->req->isRequestCompleted())
	{
		std::vector<std::string> indexes = this->server->getIndexes();
		checkServerByName();
		this->req->prepareRequest();
		if (this->req->getLocation())
		{
			if (!this->req->getLocation()->getRedirection().empty())
			{
				std::string redirection = this->req->getLocation()->getRedirection();
				throw(MovedPermanentlyException(redirection));
			}
		}
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
	struct kevent *Client::getTimeoutChangePtr() { return (this->_timeoutChangePtr); }
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
	void Client::setTimeoutChangePtr()
	{
		this->_timeoutChangePtr = new struct kevent;
	}
#endif
