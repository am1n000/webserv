/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:57:48 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/24 18:20:59 by otossa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Cgi.hpp"
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "../dev/dev.hpp"

Cgi::Cgi(Request* request): request_(request), finished_(0), in_progress(false) {}

Cgi::~Cgi() {
  for(size_t i = 0; i < this->env_.size(); i++)
    delete [] env_[i];
  remove(this->response_file_name_.c_str());
}

/*
 * file handlers
 */
void Cgi::openFiles() {
  this->response_file_name_ = std::string(TMP_VAR_PATH) + helpers::timeBasedName(".response");

  this->request_fd_ = open(this->request_->getBodyFileName().c_str(), O_RDONLY);
  this->response_fd_ = open(this->response_file_name_.c_str(), O_CREAT | O_RDWR, 0666);
  if (this->request_fd_ < 0 || this->response_fd_ < 0)
    throw InternalServerErrorException();
}

void Cgi::closeFiles() {
  close(this->request_fd_);
  close(this->response_fd_);
}

void printheaders(std::vector<char *> v) {

  for(size_t i = 0; i < v.size() - 1; i++)
    std::cout << v[i] << std::endl;
}

void Cgi::executeCgi() {
  if(!this->in_progress) {
    this->in_progress = true;
    this->openFiles();
    this->prepareEnv();
    this->prepareArgs();
    this->process_id_ = fork();
    if (this->process_id_ < 0) {
      exit(1);
      // throw InternalServerErrorException();
      // std::cerr << "Fork error" << std::endl;
    }

    if(this->process_id_ == 0) {
      dup2(this->request_fd_, 0);
      dup2(this->response_fd_, 1);
      if (execve(args_[0], &args_[0], &env_[0])) {
        exit(1);
        // throw InternalServerErrorException();
      }
    }
  }
  this->finished_  = waitpid(this->process_id_,  &this->exit_status_, WNOHANG);
  // if(!WIFEXITED(this->exit_status_)) {
  //   std::cerr << "exited with error code " << std::endl;
  //   throw InternalServerErrorException();
  // }
}


/*
* prepare cgi
*/

void Cgi::prepareEnv() {
  try {
    // common headers
    header_iterator clit = request_->getHeaders().find("Content-Type");

    env_.push_back(makeVar_("REQUEST_METHOD", Settings::methodString(request_->getRequestMethod())));
    env_.push_back(makeVar_("SCRIPT_FILENAME", this->request_->getRequestedFileFullPath()));
    env_.push_back(makeVar_("QUERY_STRING", this->request_->getQueryParams()));
    if(clit != request_->getHeaders().end())
      env_.push_back(makeVar_("CONTENT_TYPE", clit->second));
    env_.push_back(makeVar_("CONTENT_LENGTH", my_tostring(request_->getContentLength())));
    env_.push_back(makeVar_("REDIRECT_STATUS", "200"));
    if(this->request_->getLocation())
      env_.push_back(makeVar_("UPLOAD_TMP_DIR", this->request_->getLocation()->getUploadDir()));
    prepareHttpHeaders_();
    this->env_.push_back(NULL);
   // http headers 
  // dev::br();
  // for(std::map<std::string, std::string>::iterator it = this->request_->getHeaders().begin(); it != this->request_->getHeaders().end(); it++)
  //   std::cout << it->first << " " << it->second << std::endl;
  } catch (...) {
    throw InternalServerErrorException();
  }
  }

void Cgi::prepareArgs() {
  this->args_.push_back(strdup(this->request_->getRequestCgi().c_str()));
  this->args_.push_back(strdup(this->request_->getRequestedFileFullPath().c_str()));
  this->args_.push_back(NULL);
}

char *Cgi::makeVar_(std::string key, const std::string& value) {
  char *var;

  prepareKey_(key);
  size_t size = key.length() + value.length() + 2;
  var = new char[size];
  std::strcpy(var, key.c_str());
  var[key.length()] = '=';
  std::strcpy(var + key.length() + 1, value.c_str());
  var[size - 1] = '\0';
  return var;
}

void Cgi::prepareKey_(std::string &key) {
  for (size_t i = 0; i < key.length(); i++) {
    if (key[i] == '-')
      key[i] = '_';
    else
      key[i] = std::toupper(key[i]);
  }
}

void Cgi::prepareHttpHeaders_() {
  std::string http = "HTTP_";
  for(header_iterator it = request_->getHeaders().begin(); it != request_->getHeaders().end(); it++) {
    if(Cgi::common_headers.find(it->first) == Cgi::common_headers.end()) {
      env_.push_back(makeVar_(http + it->first, it->second));
    }
  }
}

/*
* getters
*/

int Cgi::getResponseFileDescriptor() { return this->request_fd_; }

std::string Cgi::getResponseFileName() { return this->response_file_name_; }

bool Cgi::isFinished() {
  return this->finished_ != 0;
}

void Cgi::setCommonHeaders() {
  size_t i = 0;
  while(Cgi::common_headers_array[i]) {
    std::cout << Cgi::common_headers_array[i] << std::endl;
    Cgi::common_headers.insert(Cgi::common_headers_array[i]);
    i++;
  }
}

const char *Cgi::common_headers_array[] = {
  "Content-Length",
  "Content-Type",
  NULL
};

std::set<std::string> Cgi::common_headers;
