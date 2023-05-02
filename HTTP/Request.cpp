/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:47:13 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/02 15:34:24 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Request.hpp"
// for test only, to be removed lather
#include "../Includes/Config.hpp"
#include "../dev/dev.hpp"
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <utility>

/*
 * constructors
 */

Request::Request()
    : header_completed_(false), body_completed_(false),
      body_file_(NULL), body_size_(0), buffer_size(BUFFER_SIZE) {
}

Request::Request(Request::size_type buffer_size)
    : header_completed_(false), body_completed_(false), body_size_(0),
      buffer_size(buffer_size) {}

Request::Request(char *buffer, Request::size_type recieved_size,
                 Request::size_type buffer_size)
    : request_string_(buffer, recieved_size), header_completed_(false),
      body_completed_(false), buffer_size(buffer_size) {}

Request::~Request() {
  if(this->body_file_) {
    if(this->body_file_->is_open())
      this->body_file_->close();
    delete this->body_file_;
  }
}
/*
 * modifiers
 */

int Request::appendBuffer(char *buffer, size_type recieved_size) {
  if(this->isHeaderCompleted())
    this->appendBodyFile(buffer, recieved_size);
  else {
    this->request_string_.append(buffer, recieved_size);
    std::string::size_type pos = this->request_string_.find(REQUEST_SEPARATOR);
    if (pos != std::string::npos) {
      this->header_completed_ = true;

      std::string body_chunk(this->request_string_.begin() + pos + 4, this->request_string_.end());
      this->appendBodyFile(&body_chunk[0], body_chunk.size());
      this->request_string_.erase(this->request_string_.begin() + pos + 4,
                                  this->request_string_.end());
    }
  }
  return this->isHeaderCompleted();
}

void Request::appendBodyFile(const char *buffer, Request::size_type size) {
  if(!this->body_file_) {
    try {
      this->body_file_ = new std::fstream;
      this->body_file_->open(TMP_FILE_NAME, std::fstream::out);
    } catch (...) {
      std::cerr << "internal server error will goes here" << std::endl;
    }
  }
  this->body_file_->write(buffer, size);
  this->body_size_ += size;
}

void Request::prepareRequest() {

  // setting up location if any match otherwise will be setted to null
  this->request_location_ =  this->matchLocation();
}

/*
 * checkers
 */

bool Request::isHeaderCompleted() {
  if (header_completed_)
    return true;
  if (this->request_string_.find(REQUEST_SEPARATOR) != std::string::npos)
    this->header_completed_ = true;
  return this->header_completed_;
}

bool Request::isBodyCompleted() {
  if(this->request_method_ != POST || this->body_completed_)
    return true;
  // if(this->body_string_.length() >= this->getContentLength()) {
  if(this->body_size_ >= this->getContentLength()) {
    this->body_completed_ = true;
    this->body_file_->close();
  }
  return this->body_completed_;
}

bool Request::isRequestCompleted() {
  return this->isHeaderCompleted() && this->isBodyCompleted();
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
  if (!this->isHeaderCompleted())
    return;
    // throw std::runtime_error("header not completed");
  std::string token;
  this->tockenizer_ = new Tockenizer(this->request_string_);
  token = this->tockenizer_->getLine();
  this->parseMetadata_(token);
  try {
    this->header_string_ = this->tockenizer_->getHeaders();
    this->pushHeaders_();
  } catch (std::runtime_error &e) {
    // just printing the error code for now
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

/*
 * private methods
 */

void Request::parseMetadata_(const std::string &metadata) {
  std::string token;
  Tockenizer tok(metadata);
  token = tok.getNextToken();
  this->setMethod(token);
  token = tok.getNextToken();
  this->setRequestUri(token);
  token = tok.getNextToken();
  this->setHttpVersion(token);
}

void Request::pushHeaders_() {
  Tockenizer tok(this->header_string_);
  while (!tok.end())
    this->request_headers_.insert(std::make_pair(
        tok.getNextToken(':'), helpers::trim(tok.getNoneEmptyLine())));
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

void Request::setHeaderString() {
  this->header_string_ = this->tockenizer_->getHeaders();
}

void Request::setMethod(const std::string &method) {
  if (method == "")
    throw BadRequestException();
  try {
    this->request_method_ = Settings::get()->indexOfRequestMethod(method);
  } catch (std::runtime_error &e) {
    // for now as place hoder i will print the error
    std::cerr << e.what() << std::endl;
    // lather on i will throw the right exception
    // that will retur the right status code to the client
  }
}

void Request::setRequestUri(const std::string &uri) {
  if (uri == "")
    throw BadRequestException();
  size_type pos = uri.find("?");
  if(pos != std::string::npos) {
    this->request_uri_ = uri.substr(0, pos);
    this->query_parameters_ = uri.substr(pos + 1);
    std::cout << this->query_parameters_ << std::endl;
    std::cout << this->request_uri_ << std::endl;
  } else {
    this->request_uri_ = uri;
    this->query_parameters_ = "";
  }
  // will be setted on the request completed hook
  this->setExtention_();
}

void Request::setHttpVersion(const std::string &version) {
  if (version == "")
    throw BadRequestException();
  this->http_version_ = version;
}

void Request::setContentLength() {
  std::map<std::string, std::string>::iterator it =
      this->request_headers_.find("Content-Length");
  if (it == this->request_headers_.end())
    this->content_length = 0;
  this->content_length = std::stoi(it->second);
}

void Request::setServer(Server *server) {
  this->server_ = server;
}

/*
 * Getters
 */

int Request::getRequestMethod() const { return this->request_method_; }

s_file Request::getFile() {
  this->file_.filename = this->server_->getRoot() + this->request_uri_;
  // std::cout << file_.filename << std::endl;
  std::string extention =
      this->file_.filename.substr(this->file_.filename.rfind(".") + 1);
  std::map<std::string, std::string>::iterator it;
  it = Config::get()->getMimeTypes().find(extention);
  if (it == Config::get()->getMimeTypes().end())
    this->file_.media = "text/plain";
  else
    this->file_.media = it->second;
  return this->file_;
}

Request::size_type Request::getContentLength() {
  // temp till i figure out where to put it
  if(this->request_method_ != POST)
    return 0;
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
  if (root[root.length() - 1] == '/')
    return root.substr(0, root.length() - 1) + this->request_uri_;
  return root + this->request_uri_;
}

/*
 * tests
 */

void Request::test() {
  // log server locations
  //
  Location* location = this->matchLocation();
  if (!location)
    std::cout << "no match" << std::endl;
  else {
      std::cout << location->getCgi().begin()->first << std::endl;
    }
}
