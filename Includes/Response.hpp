#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include "Utils.hpp"

class Response
{
	private:
		int             _bytes_to_send;
		int             _bytes_sent;
		bool            _finished;
		bool            _started;
		std::string     _filename;
		std::string     _mime_type;
		std::ifstream   _file;
	public:
		Response();
		~Response();
		void	set_file(s_file file, int sock_fd);
		int     handle_get(int sock_fd);
		int     handle_post(int sock_fd);
		int     handle_delete(int sock_fd);

};

class SendFailedException : public std::runtime_error
{
	public:
	SendFailedException() : std::runtime_error("Send operation failed") {}
};
class FileNotFound : public std::ios_base::failure
{
	public:
	FileNotFound() : std::ios_base::failure("File not found") {}
};
#endif