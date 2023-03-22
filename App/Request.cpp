#include "../Includes/Request.hpp"
#include "../Includes/Socket.hpp"
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
		this->_file.filename = "ressources/index.html";
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


int	request::receive(int kq, sock *data,struct kevent *change)
{
	int val_read = recv(data->sock_fd, this->reading_buffer, BUFFER_SIZE, 0);
	if (val_read < 0)
		throw(RecvFailedException());
	if (val_read == 0 || strstr(this->reading_buffer, "\r\n"))
	{
		EV_SET(&change[data->id], data->sock_fd, EVFILT_READ, EV_DELETE, 0, 0, data);
		if (kevent(kq, &change[data->id], 1, NULL, 0, NULL) == -1)
			std::cerr << "error: kevent 3" << std::endl;
		EV_SET(&change[data->id], data->sock_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, data);
		if (kevent(kq, &change[data->id], 1, NULL, 0, NULL) == -1)
			std::cerr << "error: kevent 33" << std::endl;
		data->filter = EVFILT_WRITE;
		data->req.parse_request_line(strtok(data->req.reading_buffer, "\r\n"));
			data->prepare_response();
	}
	return (0);
	
}