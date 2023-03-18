#include "../Includes/Socket.hpp"


//!,.................................................................. to be removed later.....................

request::request() : _method(0) , _version(0)
{}

request::~request() {}

request::request(const request &x): _method(x._method), _file(x._file), _version(x._version)
{}

request &request::operator=(const request &x)
{
	this->_method = x.get_method();
	this->_file = x.get_file();
	this->_version = x.get_version();
	return (*this);
}

int request::get_method() const
{
	return (this->_method);
}

s_file request::get_file() const
{
	return (this->_file);
}

int request::get_version() const
{
	return (this->_version);
}

std::string request::media_type(std::string extension) //to be removed, handeled by parsing config file
{		
    std::string ext[] = {"css", "csv", "gif", "htm", "html", "ico", "jpeg", "jpg", "js", "json", "png", "pdf", "mp4", "txt", "ico"};
    std::string med[] = {"text/css" , "text/csv", "image/gif", "text/html", "text/html", "image/x-con", "image/jpeg", "image/jpeg", "application/javascript", "application/json", "image/png", "application/pdf", "video/mp4", "text/plain", "image/vnd.microsoft.icon"};
    std::map<std::string, std::string> media;
    for (int i = 0; i < 15; i++)
            media.insert(std::make_pair(ext[i], med[i]));
	if (media.find(extension) != media.end())
    	return (media[extension]);
	return ("binary/octet-stream");
}
void request::set_file(char *file)
{
	std::string extension;
	this->_file.filename = file;
	if (strlen(file) == 1)
		this->_file.media = "text/html";
	else if (strchr(file, '.'))
	{	
		int pos = (this->_file.filename).find('.') + 1;
		extension = (this->_file.filename).substr(pos, (this->_file.filename).length() - pos);
		this->_file.media = media_type(extension);
	}
	else
		this->_file.media = "binary/octet-stream";

	this->_file.filename = file + 1;
	if ((this->_file.filename).length() == 0)
		this->_file.filename = "index.html";
		
	this->_file.fd = open((this->_file.filename).c_str(), O_RDONLY);
}

void	request::parse_request_line(char *line)
{
	char *method;
	char *version;

	method = strtok(line, " ");
	set_file(strtok(NULL, " "));
	version = strtok(NULL, " ");

	if (strcmp(method, "GET") == 0)
		this->_method = 1;
	else if (strcmp(method, "POST") == 0)
		this->_method = 2;
	else if (strcmp(method, "DELETE") == 0)
		this->_method = 3;
	
	if (strcmp(version, "HTTP/1.0") == 0)
		this->_version = 1;
	else if (strcmp(version, "HTTP/1.1") == 0)
		this->_version = 2;
}
//!,.................................................to be removed..........................................................

sock::sock()
{}

sock::~sock()
{}

sock::sock(sock const &obj)
{}


sock& sock::operator=(sock const &obj)
{
	return (*this);
}


void sock::create_socket()
{
	this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sock_fd == -1)
	{
		std::cerr << "error: socket" << std::endl;
		return;
	}
	fcntl(this->sock_fd, F_SETFL, O_NONBLOCK); // if not the connection will block
}

void sock::bind_socket(int port)
{
	this->host_addrlen = sizeof(this->host_addr);
	memset(&this->host_addr, 0, sizeof(this->host_addr));
	this->host_addr.sin_family = AF_INET;					//? which protocol IPv4, IPv6, ...
	this->host_addr.sin_port = htons(port);				//? the port in network byte order
	this->host_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//? the ip, use inet_addr function in case of and ip
	// .allowing the socket to be reusable..................................
	int opt = 1;
	if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "error: setsockopt" << std::endl;
		this->host_addr.sin_zero[0] = 'x';
	}
	opt = 1;
	if (setsockopt(this->sock_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "error: setsockopt" << std::endl;
		this->host_addr.sin_zero[0] = 'x';
	}
	// .binding the socket with an adress...................................
	if (bind(this->sock_fd, reinterpret_cast<struct sockaddr *>(&this->host_addr), this->host_addrlen) != 0) //? sockaddr is just an interface, is a sort of polymorphism
	{
		std::cerr << "error: bind" << std::endl;
		this->host_addr.sin_zero[0] = 'x';
	}
}

int sock::listen_to_connections()
{
    if (listen(this->sock_fd, SOMAXCONN) == -1)
    {
        std::cerr << "error: listen" << std::endl;
        return (-1);
    }
    return (0); 
}
