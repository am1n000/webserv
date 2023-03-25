#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <map>
#include <vector>
#include <poll.h>
#include <sys/event.h>
#include <cstdio>

#define BUFFER_SIZE 4096

class sock;

typedef struct t_file
{
	std::string filename;
	std::string media;
}               s_file;


class request
{
	private:
		int _method; // ? 1: GET, 2: POST, 3: DELETE
		s_file _file;
		int _version;
		std::string last_time;

	
	public:
		request();
		~request();
		request (const request &x);

		char reading_buffer[BUFFER_SIZE];

		request &operator= (const request &x);
		int             get_method() const;
		s_file			get_file() const;
		int             get_version() const;
		void			set_file(char *file);
		void            parse_request_line(char *line);
		std::string 	media_type(std::string extension);
};


class RecvFailedException : public std::runtime_error
{
    public:
    RecvFailedException() : std::runtime_error("Recv operation failed") {}
};

#endif
