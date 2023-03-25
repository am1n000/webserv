#include "../Includes/Response.hpp"


response::response() : _bytes_sent(0), _started(0), _finished(0)
{};
response::~response() {};

void response::set_file(s_file file, int sock_fd)
{
    this->_filename = file.filename;
    this->_file.open((_filename).c_str(), std::ios::binary | std::ios::ate);
    if (!this->_file.is_open())
	{
		std::string header = "HTTP/1.1 404 NOT Found\r\nServer: webserver-c";
		header += "\r\n\r\n";
		header += "<!DOCTYPE html>\n<html>\n<body>\n<h1>Skafandri: The requested URL was not found on this server.</h1>\n</body>\n</html>\r\n";
		if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
            throw(SendFailedException());
        throw(FileNotFound());
	}
    this->_mime_type = file.media;
    this->_bytes_to_send = _file.tellg();
    this->_file.seekg(0, std::ios::beg);
}

int response::upload(int sock_fd)
{
	if (this->_started == 0)
	{
		std::string header = "HTTP/1.1 200 OK\r\nServer: webserver-c\r\nContent-type: ";
		header += this->_mime_type;
		header += "\r\n";
		header += "Last-Modified: ";
		header += get_time();
		header += "\r\n\r\n";
		if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
            throw(SendFailedException());
		this->_started = 1;
		return (0);
	}
		int len = 2048;
		if(this->_bytes_sent + len >= this->_bytes_to_send)
			len = this->_bytes_to_send - this->_bytes_sent;
		char buffer[len];
		this->_file.read(buffer, len);
		if (send(sock_fd, buffer, len, 0) < 0)
            throw(SendFailedException());
		this->_bytes_sent += len;
		if (this->_bytes_sent >= this->_bytes_to_send)
		{
			this->_file.close();
            this->_finished = 1;
            return (1);
		}
    return (0);
}
