/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:47:13 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/22 22:28:13 by otossa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Request.hpp"
// for test only, to be removed lather
#include "../Includes/Config.hpp"
#include "../dev/dev.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include "../Includes/Exceptions.hpp"
#include "../Includes/Exceptions/HttpExceptions.hpp"

/*
 * constructors
 */

Request::Request()
    :tokenizer_(NULL), header_completed_(false), body_completed_(false),
      body_file_(NULL), body_size_(0), buffer_size(BUFFER_SIZE),
      request_location_(NULL),
      chunk_size_(0), chunk_received_(0),
      content_length(0)
{}

Request::Request(Request::size_type buffer_size)
    : tokenizer_(NULL), header_completed_(false), body_completed_(false), body_size_(0),
      buffer_size(buffer_size),
      request_location_(NULL),
      chunk_size_(0), chunk_received_(0),
      content_length(0)
{}

Request::Request(char *buffer, Request::size_type recieved_size,
                 Request::size_type buffer_size)
    : tokenizer_(NULL), request_string_(buffer, recieved_size), header_completed_(false),
      body_completed_(false), buffer_size(buffer_size),
      request_location_(NULL),
      chunk_size_(0), chunk_received_(0),
      content_length(0)
{}

Request::~Request() {
  if(this->body_file_) {
	if(this->body_file_->is_open())
	  this->body_file_->close();
  // if(this->request_location_)
  //     delete this->request_location_;
  if(this->tokenizer_)
      delete this->tokenizer_;
	delete this->body_file_;
	remove(this->body_file_name_.c_str());
  }
}
/*
 * modifiers
 */

int Request::appendBuffer(char *buffer, size_type recieved_size) {
  if (this->isHeaderCompleted()) {
    if(this->isChuncked()){
      std::string stringBuffer(buffer, recieved_size);
      this->unchunckRequest(stringBuffer);
    } else
      this->appendBodyFile(buffer, recieved_size);
  }
  else {
    this->request_string_.append(buffer, recieved_size);
    if (this->isHeaderCompleted()) {
      std::string::size_type pos =
          this->request_string_.find(REQUEST_SEPARATOR);

      std::string body_chunk(this->request_string_.begin() + pos + 4,
                             this->request_string_.end());
      if (this->isChuncked()) {
          this->unchunckRequest(body_chunk);
      } else {
          this->appendBodyFile(&body_chunk[0], body_chunk.size());
          this->request_string_.erase(this->request_string_.begin() + pos + 4,
                                      this->request_string_.end());
        }
    }
  }
  return this->isHeaderCompleted();
}

void Request::appendBodyFile(const char *buffer, Request::size_type size) {
  if(!this->body_file_) {
	try {
	  this->body_file_name_ = std::string(TMP_VAR_PATH) + helpers::timeBasedName(".request");
	  this->body_file_ = new std::fstream;
	  this->body_file_->open(this->body_file_name_.c_str(), std::fstream::out);
	} catch (...) {
	  std::cerr << "internal server error will goes here" << std::endl;
	}
  }
  this->body_file_->write(buffer, size);
  this->body_size_ += size;
}

void Request::unchunckRequest(std::string& buffer) {
  size_type size_to_write(0);
  // begining of new chunk
  if(this->chunk_received_ == 0)
  {
    // checking if a chunk in the middle to remove chunk separator
    if (buffer.find("\r\n") == 0)
    {
      buffer.erase(0, 2);
    }
    size_type pos = buffer.find("\r\n");
    this->chunk_size_ = helpers::stoi(buffer.substr(0, pos));
    // body completed if chunk size equal zero
    buffer.erase(0, pos + 2);
    if(this->chunk_size_ == 0)
    {
      this->body_completed_ = true;
      this->content_length = body_size_;
      if(this->body_file_->is_open())
        this->body_file_->close();
      return;
    }
    size_to_write = (this->chunk_size_ < buffer.length() + chunk_received_ ? this->chunk_size_ - this->chunk_received_ : buffer.length());
    this->appendBodyFile(buffer.data(), size_to_write);
    this->chunk_received_ += size_to_write;
    if(size_to_write < buffer.length())
    {
      buffer.erase(0, size_to_write);
      this->chunk_received_ = 0;
      this->chunk_size_ = 0;
      this->unchunckRequest(buffer);
    }
  }
  // resuming old chunk
  else
  {
    if (buffer.length() + this->chunk_received_ < this->chunk_size_)  
    {
      this->appendBodyFile(buffer.data(), buffer.length());
      this->chunk_received_ += buffer.length();
    }
    else 
    {
      size_to_write = this->chunk_size_ - this->chunk_received_;
      this->appendBodyFile(buffer.data(), size_to_write);
      buffer.erase(0, size_to_write);
      this->chunk_size_ = 0;
      this->chunk_received_ = 0;
      this->unchunckRequest(buffer);
    }
  }
}

bool Request::outOfRoot()
{
    std::string root = this->request_location_->getRoot();
    std::string ressource = this->getRequestedRessource();
    std::vector<std::string> paths = splitPaths(this->getRequestedRessource());
    std::vector<std::string> rootPaths = splitPaths(this->request_location_->getRoot());
    return (withinRoot(paths, rootPaths));
}

void Request::prepareRequest() {

  // setting up location if any match otherwise will be setted to null
  this->request_location_ =  this->matchLocation();
  if (this->outOfRoot())
    throw (BadRequestException());
}

/*
 * checkers
 */

bool Request::isHeaderCompleted() {
  if (header_completed_)
    return true;
  if (this->request_string_.find(REQUEST_SEPARATOR) != std::string::npos) {
    this->header_completed_ = true;
    this->headerCompletedEventHook();
  }
  return this->header_completed_;
}

bool Request::isBodyCompleted() {
  if(this->request_method_ != POST || this->body_completed_)
    return true;
  if(this->isChuncked())
    return this->body_completed_;
  if(this->body_size_ >= this->getContentLength()) {
	this->body_completed_ = true;
	this->body_file_->close();
  }
  return this->body_completed_;
}

bool Request::isRequestCompleted() {
  return this->isHeaderCompleted() && this->isBodyCompleted();
}

bool Request::hasCgi() const {
  if(this->request_location_) {
	std::map<std::string, std::string>::iterator it =
		this->request_location_->getCgis().find(this->extention_);
	if(it != this->request_location_->getCgis().end())
	  return true;
  }
  return false;
}

bool Request::isAutoIndexed() {
  if(!this->request_location_)
    this->request_location_ = this->matchLocation();
  return this->request_location_->getAutoIndex();
}

bool Request::isChuncked() {
  string_map_type::iterator it = this->request_headers_.find("Transfer-Encoding");
  if (it != this->request_headers_.end()) {
    if (it->second != "Chunked") 
      throw NotImplementedException();
    return true;
  }
  return false;
}

bool Request::isMethodAllowed() {
  if(this->request_location_) {
std::vector<Request_Method_e>& allowed_methods = this->request_location_->getAllowedMethods();
    if(allowed_methods.size() == 0)
      return true;
    for (std::vector<Request_Method_e>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); it++) {
      if(*it == this->request_method_)
         return true;
    }
    return false;
  }
  return true;
}

Location* Request::matchLocation() {
  size_type pos;
  std::string locationString;
  Server::location_type location = this->server_->getLocations();
  for(Server::location_iterator it = location.begin(); it != location.end(); it++) {
	pos = this->request_uri_.find(it->first);
	if(pos != std::string::npos && locationString.length() < it->first.length())
	  locationString = it->first;
  }
  if(locationString.length() > 0) {
	Server::location_iterator lit = this->server_->getLocations().find(locationString);
	// removing mathed location from the request uri
	this->request_uri_.erase(0, lit->first.length());
	return lit->second;
  }
  return NULL;
}

/*
 * parsers
 */

void Request::parseHeader() {
  if(!this->isHeaderCompleted())
    return;
  this->tokenizer_ = new RequestTokenizer(this->request_string_);
  this->parseMetadata_(this->tokenizer_->getLine());
  this->request_headers_ = this->tokenizer_->parseHeaders();
}

/*
 * private methods
 */

void Request::parseMetadata_(const std::string &metadata) {
  // form should  be checked
  std::string token;
  Tockenizer tok(metadata);
  token = tok.getNextToken();
  this->setMethod(token);
  token = tok.getNextToken();
  this->setRequestedRessource(token);
  this->setRequestUri(token);
  token = tok.getNextToken();
  this->setHttpVersion(token);
}

void Request::pushHeaders_() {
  Tockenizer tok(this->header_string_);
  std::string key;
  std::string value;
  while (!tok.end()) {
  key = tok.getNextToken(':'); 
  value = helpers::trim(tok.getNoneEmptyLine());
  if(helpers::hasSpace(key))
      throw BadRequestException();
	this->request_headers_.insert(std::make_pair(key, value));
  }
}

void Request::setExtention_() {
  size_type pos;

  pos = this->request_uri_.rfind("/");
  if(pos != std::string::npos) {
	this->filename_ = this->request_uri_.substr(pos + 1);
	pos = this->filename_.rfind(".");
	if(pos != std::string::npos)
	  this->extention_ = this->filename_.substr(pos + 1);
  }
}

/*
 * setters
 */

void Request::setMethod(const std::string &method) {
  if (method.length() == 0)
	  throw BadRequestException();
	this->request_method_ = Settings::get()->indexOfRequestMethod(method);
  if(!this->isMethodAllowed())
      throw MethodNotAllowedException();
}

void Request::setRequestUri(const std::string &uri) {
  if (uri == "")
	throw BadRequestException();
  size_type pos = uri.find("?");
  if(pos != std::string::npos) {
    this->request_uri_ = uri.substr(0, pos);
    this->query_parameters_ = uri.substr(pos + 1);
  } else {
	this->request_uri_ = uri;
	this->query_parameters_ = "";
  }
  // will be setted on the request completed hook
  this->setExtention_();
}

void Request::setRequestedRessource(const std::string &uri) {
  if(uri.length() == 0)
    throw BadRequestException();
	this->requestedRessource_ = uri;
}

void Request::setHttpVersion(const std::string &version) {
  if (version.length() == 0)
	  throw BadRequestException();
  this->http_version_ = version;
}

void Request::setContentLength() {
  std::map<std::string, std::string>::iterator it =
	  this->request_headers_.find("Content-Length");
  if (it == this->request_headers_.end())
    this->content_length = 0;
  else
    this->content_length = std::atoi(it->second.data());
}

void Request::setServer(Server *server) {
  this->server_ = server;
}

/*
 * Getters
 */


int Request::getRequestMethod() const { return this->request_method_; }

Request::size_type Request::getContentLength() {
  // temp till i figure out where to put it
  if(this->request_method_ != POST)
    return 0;
  if(!this->isChuncked())
    this->setContentLength();
  return this->content_length;
}

std::string Request::getRequestString() const { return this->request_string_; }

std::string Request::getBodyString() const { return this->body_string_; }

std::string Request::getHeaderString() const { return this->header_string_; }

Request::size_type Request::getBodySize() const { return this->body_size_; }

std::map<std::string, std::string> &Request::getHeaders() {
  return this->request_headers_;
}

const std::string &Request::getRequestUri() { return this->request_uri_; }

const std::string &Request::getRequestedRessource() { return this->requestedRessource_; }

std::string Request::getExtention() const { return this->extention_; }

Server *Request::getServer() const { return this->server_; }

std::map<std::string, Location *> &Server::getLocations() { return this->locations_; }

std::string Request::getRequestRoot() const {
  if (this->request_location_ != NULL) {
	if (this->request_location_->getRoot().size() != 0)
	  return this->request_location_->getRoot();
  }
  return this->server_->getRoot();
}

std::string Request::getRequestedFileFullPath() const {
  std::string root = this->getRequestRoot();
  return root + this->request_uri_;
}

std::string Request::getMimeType() const {
  std::map<std::string, std::string>::iterator it;
  it = Config::get()->getMimeTypes().find(this->getExtention());
  if (it == Config::get()->getMimeTypes().end())
	return "text/plain";
  return it->second;
}

std::string &Request::getRequestCgi() const {
  if(this->request_location_) {
	std::map<std::string, std::string>::iterator it =
		this->request_location_->getCgis().find(this->extention_);
	if(it != this->request_location_->getCgis().end())
	  return it->second;
  }
  throw std::runtime_error("trying to get cgi but not set, check config file or contact otossa for more details");
}

std::fstream *Request::getBodyFile() { return this->body_file_; }

std::string &Request::getBodyFileName() { return this->body_file_name_; }

std::string& Request::getQueryParams() { return this->query_parameters_; }

Location *Request::getLocation() { return this->request_location_; }

/*
 * END getters
 */

/*
 * hooks
 */

void Request::headerCompletedEventHook() {
  this->parseHeader();
}

/*
 * tests
 */

void Request::test() {
}
