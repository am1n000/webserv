/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:47:13 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/11 02:34:04 by hchakoub         ###   ########.fr       */
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
    // this->appendBodyFile(buffer, recieved_size);
    this->body_string_.append(buffer, recieved_size);
  else {
    this->request_string_.append(buffer, recieved_size);
    std::string::size_type pos = this->request_string_.find(REQUEST_SEPARATOR);
    if (pos != std::string::npos) {
      if  (this->request_method_ != POST)
        return 1;
      this->header_completed_ = true;
      this->body_string_.append(this->request_string_.begin() + pos + 4,
                                this->request_string_.end());
      // std::string body_chunk(this->request_string_.begin() + pos + 4, this->request_string_.end());
        // this->appendBodyFile(&body_chunk[0], body_chunk.length());
      this->request_string_.erase(this->request_string_.begin() + pos + 4,
                                  this->request_string_.end());
    }
  }
  if(isBodyCompleted() )
    std::cout << "body is completed" << std::endl;
if(this->request_method_ == POST && isBodyCompleted()){
    if(!this->body_file_)
      this->body_file_ = new std::fstream;
    if(!this->body_file_->is_open())
      this->body_file_->open(TMP_FILE_NAME, std::ios::out);

    if (this->body_file_->is_open())
      std::cout << "file is openned" << std::endl;
    this->body_file_->write(static_cast<const char *>(&this->body_string_[0]), this->body_string_.length());
    this->body_file_->close();
  }
  return this->isHeaderCompleted();
}

void Request::appendBodyFile(const char *buffer, Request::size_type size) {
  if(this->request_method_ != POST)
    return;
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
  if(this->request_method_ != POST)
    return true;
  if (this->body_completed_)
    return this->body_completed_;
  // if(this->body_size_ >= this->getContentLength()) {
  if(this->body_string_.length() >= this->getContentLength()) {
    this->body_completed_ = true;
    // this->test();
  }
  return this->body_completed_;
}

bool Request::isRequestCompleted() {
  return this->isHeaderCompleted() && this->isBodyCompleted();
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
  this->request_uri_ = uri;
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

/*
 * Getters
 */

int Request::getRequestMethod() const { return this->request_method_; }

s_file Request::getFile() {
  // all this **** is temprary to make the **** works
  this->file_.filename =
      "/Users/hchakoub/cursus/webserv/ressources" + this->request_uri_;
  std::string extention =
      this->file_.filename.substr(this->file_.filename.find(".") + 1);
  std::map<std::string, std::string>::iterator it;
  it = Config::get()->getMimeTypes().find(extention);
  if (it == Config::get()->getMimeTypes().end())
    this->file_.media = "application/octet-stream";
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

std::map<std::string, std::string>& Request::getHeaders() {
return this->request_headers_;
}

/*
 * tests
 */

void Request::test() {
  // if(this->request_method_ == POST) {
  // std::fstream file("/Users/hchakoub/cursus/webserv/ressources/uploads/file.out", std::fstream::out);
  // file.write(this->body_string_.data(), this->body_string_.length());
  //   dev::br();
  //   dev::br();
  // }
}
