#include "../Includes/Response.hpp"
#include "../Includes/Cgi.hpp"
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

Response::Response(Request* request) : _request(request),  _cgi_fd(-2), _bytes_sent(0), _finished(0), _started(0), _hasCgi(false), _cgi(NULL)
{
  // if(this->_request->hasCgi()) {
    this->_cgi = new Cgi(this->_request);
    // this->_hasCgi = true;
  // }
};

Response::Response() : _cgi_fd(-2), _bytes_sent(0), _finished(0), _started(0), _cgi(NULL)
{
  // if(this->_request->hasCgi()) {
    // this->_hasCgi = true;
  // }
};

Response::~Response()
{
  if(this->_cgi)
	  delete (this->_cgi);
};

void Response::set_file(std::string path)
{
	this->_file.open(path.c_str(), std::ios::binary | std::ios::ate);
	if (!this->_file.is_open())
		throw(FileNotFoundException());
	std::string extention = path.substr(path.rfind(".") + 1);
	std::map<std::string, std::string>::iterator it;
	it = Config::get()->getMimeTypes().find(extention);
	if (it == Config::get()->getMimeTypes().end())
		this->_mime_type = "text/plain";
	else
		this->_mime_type = it->second;
	this->_bytes_to_send = _file.tellg();
	this->_file.seekg(0, std::ios::beg);
  if(this->_request->hasCgi()) {
    this->_hasCgi = true;
    this->_cgi = new Cgi(this->_request);
  }
}


int Response::handle_get(int sock_fd)
{
	if(this->_request->hasCgi())
		return (this->handleCgi(sock_fd));
	if (this->_started == 0)
	{
		std::string dirCheck = directoryCheck(sock_fd);
		if (dirCheck.size() == 0)
		{
			this->_finished = 1;
			return (1);
		}
		this->set_file(dirCheck);
		std::string header = "HTTP/1.1 200 OK\r\nServer: webserver-c\r\nContent-type: ";
		header += this->_mime_type;
		header += "\r\n";
		header += "Last-Modified: ";
		header += get_time();
		header += "\r\n\r\n";
		if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
			throw(InternalServerErrorException());
		this->_started = 1;
		return (0);
	}
  	int bufferSize = 2048;
		if(this->_bytes_sent + bufferSize >= this->_bytes_to_send)
			bufferSize = this->_bytes_to_send - this->_bytes_sent;
		char buffer[bufferSize];
		this->_file.read(buffer, bufferSize);
		if (send(sock_fd, buffer, bufferSize, 0) < 0)
			throw(InternalServerErrorException());
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
        if (this->_request->hasCgi())
                        return (this->handleCgi(sock_fd));
        else
                        std::cerr << "no cgi " << std::endl;
        std::string header = "HTTP/1.1 201 Created\r\nLocation: "
                             "/resources/post\t\nContent-Type: "
                             "text/plain\r\n\r\nrequest has been posted";
        if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
                        throw(InternalServerErrorException());
        return (1);
}

int Response::handle_delete(int sock_fd)
{
	if (this->_request->hasCgi())
		return (this->handleCgi(sock_fd));

    struct stat fileStat;
	std::string directoryFullPath = this->_request->getRequestedFileFullPath();
    if (stat(directoryFullPath.c_str(), &fileStat) == 0)
    {
        if (S_ISDIR(fileStat.st_mode))
        {
            DIR* dirp = opendir(directoryFullPath.c_str());
            if (dirp == NULL)
                throw (ForbiddenException());
            if (this->_request->getRequestedRessource()[this->_request->getRequestedRessource().size() - 1] != '/')
			{
                throw (ConflictException());
			}
		}
		if (std::remove(this->_request->getRequestedFileFullPath().c_str()) != 0)
			throw(ForbiddenException());
		std::string header = "HTTP/1.1 204 No Content\r\n\r\n";
		if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
			throw(InternalServerErrorException());
		return (1);
	}
	throw(FileNotFoundException());
	return (0);
}


bool Response::handleCgi(int sock_fd)
{
  if(!this->_cgi)
    throw InternalServerErrorException();
  // openning the files for the first time
  if (!this->cgiInProgress_())
  {
    std::fstream f(this->_request->getRequestedFileFullPath().c_str());
    if(!f.good())
    {
      if (f.is_open())
      f.close();
      throw FileNotFoundException();
    }
		if (f.is_open())
		f.close();
  }
	this->_cgi->executeCgi();
  if(this->_cgi->isFinished()) {
    // std::cout << "send portion" << std::endl;
    if(this->_cgi_fd == -2) {
      this->_cgi->closeFiles();
      this->_cgi_fd = open(this->_cgi->getResponseFileName().c_str(), O_RDONLY);
      std::string header = "HTTP/1.1 201 Created\r\nLocation: /resources/post";
      if (send(sock_fd, header.c_str(), header.length(), 0) < 0)
        throw(InternalServerErrorException());
    }

    int size = 1024;
    char buffer[size];
    int i = 0;
    i = read(this->_cgi_fd, buffer, size);
    if (i == -1)
      throw(InternalServerErrorException());
    if (i == 0)
    {
      close (this->_cgi_fd);
      return (1);
    }
    buffer[i] = 0;
    if (send(sock_fd, buffer, i, 0) < 0)
      throw(InternalServerErrorException());
    }

	return (0);
}


//. Directory and indexing related functions


std::vector<std::string> Response::chooseIndexes()
{
	if (this->_request->getLocation() != NULL && (this->_request->getLocation())->getIndex().size() != 0)
		return ((this->_request->getLocation())->getIndex());
	return (this->_request->getServer()->getIndexes());

}

std::vector<std::string>	Response::getDirectoryContent(DIR* dirp)
{
    struct dirent* dp;
	std::vector<std::string> content;
	while ((dp = readdir(dirp)) != NULL)
	{
		if (dp->d_name[0] != '.') // Skip hidden files and directories
			content.push_back(dp->d_name);
	}
	return (content);
}

std::string	Response::indexCheck(std::vector<std::string> content)
{
	std::vector<std::string> indexes = chooseIndexes();
	for (size_t i = 0; i < indexes.size(); i++)
	{
		for (size_t j = 0; j < content.size(); j++)
		{
			if (indexes[i] == content[j])
				return (indexes[i]);
				
		}
	}
	return ("");
}

std::string	Response::directoryCheck(int sock_fd)
{
    struct stat fileStat;
	std::string directoryFullPath = this->_request->getRequestedFileFullPath();
    if (stat(directoryFullPath.c_str(), &fileStat) == 0)
    {
        if (S_ISDIR(fileStat.st_mode))
        {
            DIR* dirp = opendir(directoryFullPath.c_str());
            if (dirp == NULL)
                throw (ForbiddenException());
            if (this->_request->getRequestedRessource()[this->_request->getRequestedRessource().size() - 1] != '/')
                throw (MovedPermanentlyException());	
			std::string indexFile = "HTTP/1.1 200 OK\r\nServer: Almo7arrikX\r\nContent-type: text/html\r\nLast-Modified: ";
			indexFile += get_time();
			indexFile += "\r\n\r\n";
			indexFile += "<!DOCTYPE html>\n<html>\n<head>\n<title>Index</title>\n</head>\n<body>\n<h1>Index of " + this->_request->getRequestedRessource();
			indexFile += "</h1>\n<table style=\"text-align: left;\">\n<tr>\n<th>Name</th>\n<th>Last modified</th>\n<th>Size</th>\n</tr>\n";
			std::vector<std::string> content = this->getDirectoryContent(dirp);
			std::string checkIndex = this->indexCheck(content);
			if (checkIndex.size())
				return (directoryFullPath + "/" + checkIndex);
            if (this->_request->isAutoIndexed() == false)
                throw (ForbiddenException());
			for (size_t i = 0; i < content.size(); i++)
			{
				std::string filePath = directoryFullPath + "/" + content[i];
				if (stat(filePath.c_str(), &fileStat) == 0) // Check if stat succeeded
				{
					std::time_t modified_time = fileStat.st_mtime;
					std::tm* modified_tm = std::localtime(&modified_time);
					char modified_str[20];
					std::strftime(modified_str, sizeof(modified_str), "%Y-%m-%d %H:%M:%S", modified_tm);
					indexFile += "<tr>\n<td><a href=\"" + this->_request->getRequestedRessource();
					indexFile += content[i] + "\">";
					indexFile += content[i] + "</a></td>\n";
					indexFile += "<td>";
					indexFile += modified_str;
					indexFile += "</td>\n";
					indexFile += "<td>";
					indexFile += my_tostring(fileStat.st_size);
					indexFile += "</td>\n</tr>\n";
				}
			}
            indexFile += "	</table>\n</body>\n</html>";
			if (send(sock_fd, indexFile.c_str(), indexFile.length(), 0) < 0)
				throw(InternalServerErrorException());
			return ("");
		}
		return (this->_request->getRequestedFileFullPath());
	}
	return (this->_request->getRequestedFileFullPath());
}


bool Response::cgiInProgress_() {
  if(this->_cgi && this->_cgi->in_progress)
    return true;
  return false;
}
