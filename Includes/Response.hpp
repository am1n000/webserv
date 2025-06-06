#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include "Config.hpp"
#include "Cgi.hpp"

class Response
{
	private:
    	Request  					*_request;
		int							_cgi_fd;
		int             			_bytes_to_send;
		int             			_bytes_sent;
		bool            			_finished;
		bool            			_started;
    	bool            			_hasCgi;
		std::string     			_filename;
		std::string     			_mime_type;
		std::ifstream   			_file;
		Cgi							*_cgi;
    std::string           _cgi_buffer;
		

	private:
		//indexing functions
  	std::vector<std::string> 		chooseIndexes();
		std::vector<std::string>		getDirectoryContent(DIR* dirp);
		std::string 					directoryCheck(int sock_fd);
		std::string 					indexCheck(std::vector<std::string> content);
    bool                        cgiInProgress_();
    void                        manipulateHeaderByMethod(std::string& header);

	public:
		Response();
		Response(Request *request);
		~Response();
    bool respond(int sockFd);
		void	set_file(std::string path);
		int     handle_get(int sock_fd);
		int     handle_post(int sock_fd);
		int     handle_delete(int sock_fd);
    	bool    handleCgi(int sock_fd);

};

#endif
