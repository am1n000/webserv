#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include "Utils.hpp"

class Response
{
	private:
    const Request  *_request;
		int             _bytes_to_send;
		int             _bytes_sent;
		bool            _finished;
		bool            _started;
		std::string     _filename;
		std::string     _mime_type;
		std::ifstream   _file;
	public:
		Response();
		Response(const Request *request);
		~Response();
		void	set_file(int sock_fd);
		int     handle_get(int sock_fd);
		int     handle_post(int sock_fd);
		int     handle_delete(int sock_fd);

};

class SendFailedException : public std::runtime_error
{
	public:
	SendFailedException() : std::runtime_error("Send operation failed") {}
};
#endif
