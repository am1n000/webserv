#include "../Includes/Response.hpp"
#include "../Includes/Cgi.hpp"
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

Response::Response(Request* request) : _request(request), _bytes_sent(0), _finished(0), _started(0), _hasCgi(false)
{};

Response::Response() :_bytes_sent(0), _finished(0), _started(0)
{};

Response::~Response() {};

void Response::set_file()
{
    this->_file.open((this->_request->getRequestedFileFullPath()).c_str(), std::ios::binary | std::ios::ate);
    if (!this->_file.is_open())
        throw(FileNotFound());
    this->_mime_type = this->_request->getMimeType();
    this->_bytes_to_send = _file.tellg();
    this->_file.seekg(0, std::ios::beg);
    if (this->_request->hasCgi())
      this->_hasCgi = true;
}

int Response::handle_get(int bufferSize, int sock_fd)
{
  if(this->_request->hasCgi()) {
    this->handleCgi(sock_fd);
    return 1;
  }

	if (this->_started == 0)
	{
		std::string header = "HTTP/1.1 200 OK\r\nServer: webserver-c\r\nContent-type: ";
		header += this->_mime_type;
		header += "\r\n";
		header += "Last-Modified: ";
		header += get_time();
		header += "\r\n\r\n";
		if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
            throw(InternalServerError());
		this->_started = 1;
		return (0);
	}
		if(this->_bytes_sent + bufferSize >= this->_bytes_to_send)
			bufferSize = this->_bytes_to_send - this->_bytes_sent;
		char buffer[bufferSize];
		this->_file.read(buffer, bufferSize);
		if (send(sock_fd, buffer, bufferSize, 0) < 0)
    {
            std::cout << " ...................................  " << sock_fd << std::endl;
            throw(InternalServerError());
    }
		this->_bytes_sent += bufferSize;
		if (this->_bytes_sent >= this->_bytes_to_send)
		{
			  this->_file.close();
        this->_finished = 1;
        return (1);
		}
    return (0);
}

int Response::handle_post(int sock_fd)
{
  if (this->_request->hasCgi()) {
    this->handleCgi(sock_fd);
    return (1);
  }
  else 
    std::cout << "no cgi " << std::endl;
	std::string header = "HTTP/1.1 201 Created\r\nLocation: /resources/post\t\nContent-Type: text/plain\r\n\r\nrequest has been posted";
	if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
		throw(InternalServerError());
	return (1);
}

int Response::handle_delete(int sock_fd)
{
	std::string header = "HTTP/1.1 204 No Content\r\n\r\n";
	if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
		throw(InternalServerError());
	if (std::remove(this->_filename.c_str()) != 0)
		std::cerr << "error : file deletion" << std::endl;
	return (1);
}


void Response::handleCgi(int sock_fd) {
  int size = 1024;
  Cgi cgi(this->_request);
  cgi.executeCgi();

  cgi.closeFiles();
  int fd = open(cgi.getResponseFileName().c_str(), O_RDONLY);

  char buffer[size];
  
  int i = 0;
	std::string header = "HTTP/1.1 201 Created\r\nLocation: /resources/post";
  send(sock_fd, header.c_str(), header.length(), 0);
  /*
    * this loop blocking the server's multiplexing
    * working now only for test
    * will be handled lather
    */
  do {
    i = read(fd, buffer, size);
    buffer[i] = 0;
    send(sock_fd, buffer, i, 0);
  } while (i > 0);
}

