/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:57:48 by hchakoub          #+#    #+#             */
/*   Updated: 2023/06/06 14:22:06 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Cgi.hpp"
#include "../dev/dev.hpp"
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

Cgi::Cgi(Request *request)
    : request_(request), finished_(0), in_progress(false) {}

Cgi::~Cgi() {
  for (size_t i = 0; i < this->env_.size(); i++)
    delete[] env_[i];
  for (size_t i = 0; i < this->args_.size(); i++)
    delete[] args_[i];
  remove(this->response_file_name_.c_str());
}

/*
 * file handlers
 */
void Cgi::openFiles() {
  this->response_file_name_ =
      std::string(TMP_VAR_PATH) + helpers::timeBasedName(".response");

  this->request_fd_ = open(this->request_->getBodyFileName().c_str(), O_RDONLY);
  this->response_fd_ =
      open(this->response_file_name_.c_str(), O_CREAT | O_RDWR, 0666);
  if (this->request_fd_ < 0 || this->response_fd_ < 0)
    throw InternalServerErrorException();
}

void Cgi::closeFiles() {
  close(this->request_fd_);
  close(this->response_fd_);
}

void Cgi::executeCgi() {
  if (!this->in_progress) {
    this->in_progress = true;
    this->openFiles();
    this->prepareEnv();
    this->prepareArgs();
    this->process_id_ = fork();
    if (this->process_id_ < 0) {
      exit(1);
    }

    if (this->process_id_ == 0) {
      dup2(this->request_fd_, 0);
      dup2(this->response_fd_, 1);
      if (execve(args_[0], &args_[0], &env_[0])) {
        exit(1);
      }
    }
  }
  this->finished_ = waitpid(this->process_id_, &this->exit_status_, WNOHANG);
  if (WEXITSTATUS(this->exit_status_)) {
    if (WIFEXITED(this->exit_status_) == 1) {
      std::cerr << "error when trying to execute cgi" << std::endl;
      throw InternalServerErrorException();
    }
  }
}

/*
 * prepare cgi
 */

void Cgi::prepareEnv() {
  try {
    // common headers
    header_iterator clit = request_->getHeaders().find("Content-Type");

    env_.push_back(makeVar_("REQUEST_METHOD",Settings::methodString(request_->getRequestMethod())));
    env_.push_back(makeVar_("SCRIPT_FILENAME", this->request_->getRequestedFileFullPath()));
    env_.push_back(makeVar_("QUERY_STRING", this->request_->getQueryParams()));
    if (clit != request_->getHeaders().end())
      env_.push_back(makeVar_("CONTENT_TYPE", clit->second));
    env_.push_back(makeVar_("CONTENT_LENGTH", helpers::to_string(request_->getContentLength())));
    env_.push_back(makeVar_("REDIRECT_STATUS", "200"));
    if (this->request_->getLocation())
      env_.push_back(makeVar_("UPLOAD_TMP_DIR", this->request_->getLocation()->getUploadDir()));
    prepareHttpHeaders_();
  /*
   *  SERVER_PROTOCOL , SERVER_PORT, SERVER_NAME, SERVER_SOFTWARE
   */

    env_.push_back(makeVar_("SERVER_PROTOCOL", "HTTP 1.1"));
    env_.push_back(makeVar_("SERVER_PORT", this->request_->getServer()->getPort()));
    env_.push_back(makeVar_("SERVER_NAME", this->request_->getServer()->getServerName()));
    env_.push_back(makeVar_("SERVER_SOFTWARE", "l'mo7arik x"));
    this->env_.push_back(NULL);
  } catch (...) {
    throw InternalServerErrorException();
  }
}

void Cgi::prepareArgs() {

  char *cgi = new char[this->request_->getRequestCgi().length() + 1];
  char *script = new char[this->request_->getRequestedFileFullPath().length() + 1];
  std::strcpy(cgi, this->request_->getRequestCgi().c_str());
  std::strcpy(script, this->request_->getRequestedFileFullPath().c_str());
  this->args_.push_back(cgi);
  this->args_.push_back(script);
  this->args_.push_back(NULL);
}

char *Cgi::makeVar_(std::string key, const std::string &value) {
  char *var;

  prepareKey_(key);
  size_t size = key.length() + value.length() + 2;
  var = new char[size];
  std::strcpy(var, key.c_str());
  var[key.length()] = '=';
  std::strcpy(var + key.length() + 1, value.c_str());
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
  for (header_iterator it = request_->getHeaders().begin();
       it != request_->getHeaders().end(); it++) {
    if (Cgi::common_headers.find(it->first) == Cgi::common_headers.end()) {
      env_.push_back(makeVar_(http + it->first, it->second));
    }
  }
}

/*
 * getters
 */

int Cgi::getResponseFileDescriptor() { return this->request_fd_; }

std::string Cgi::getResponseFileName() { return this->response_file_name_; }

bool Cgi::isFinished() { return this->finished_ != 0; }

void Cgi::setCommonHeaders() {
  size_t i = 0;
  while (Cgi::common_headers_array[i]) {
    Cgi::common_headers.insert(Cgi::common_headers_array[i]);
    i++;
  }
}

const char *Cgi::common_headers_array[] = {"Content-Length", "Content-Type",
                                           NULL};

std::set<std::string> Cgi::common_headers;
