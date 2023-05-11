#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include "Utils.hpp"

class Response
{
	private:
    Request  *_request;
		int             _bytes_to_send;
		int             _bytes_sent;
		bool            _finished;
		bool            _started;
		std::string     _filename;
		std::string     _mime_type;
		std::ifstream   _file;
    bool            _hasCgi;
	public:
		Response();
		Response(Request *request);
		~Response();
		void	set_file();
		int     handle_get(int bufferSize, int sock_fd);
		int     handle_post(int sock_fd);
		int     handle_delete(int sock_fd);
    void    handleCgi(int sock_fd);

};

#endif
