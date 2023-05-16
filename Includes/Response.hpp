#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include "Utils.hpp"
#include "Config.hpp"

class Response
{
	private:
    	Request  					*_request;
		int             			_bytes_to_send;
		int             			_bytes_sent;
		bool            			_finished;
		bool            			_started;
    	bool            			_hasCgi;
		std::string     			_filename;
		std::string     			_mime_type;
		std::ifstream   			_file;

	private:
		//indexing functions
  		std::vector<std::string> 		chooseIndexes();
		std::vector<std::string>		getDirectoryContent(DIR* dirp);
		std::string 					directoryCheck(int sock_fd);
		std::string 					indexCheck(std::vector<std::string> content);

	public:
		Response();
		Response(Request *request);
		~Response();
		void	set_file(std::string path);
		int     handle_get(int sock_fd);
		int     handle_post(int sock_fd);
		int     handle_delete(int sock_fd);
    	void    handleCgi(int sock_fd);

};

#endif
