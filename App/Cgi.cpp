/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:57:48 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/23 15:06:00 by otossa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Cgi.hpp"
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

// void Cgi::appendStream(const char *buffer, size_t buffer_size) {
//   // if(!this->file_.is_open())
//   //   throw std::runtime_error("file is not oppend");
//   // this->file_.write(buffer, buffer_size);
// }

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
  // the strdup thing will be handled lather it is just a tmp thing
  try {
    std::string cl(my_tostring(request_->getContentLength()));
    std::string tmp = "CONTENT_LENGTH=" + cl;
    this->env_.push_back(strdup(tmp.data()));
    tmp = "CONTENT_TYPE=" + request_->getHeaders().find("Content-Type")->second;
    this->env_.push_back(strdup(tmp.data()));
    tmp = "REQUEST_METHOD=" + Settings::methodString(request_->getRequestMethod());
    this->env_.push_back(strdup(tmp.data()));
    tmp = "SCRIPT_FILENAME=";
    tmp += this->request_->getRequestedFileFullPath();
    this->env_.push_back(strdup(tmp.data()));
    tmp = "QUERY_STRING=" + this->request_->getQueryParams();
    this->env_.push_back(strdup(tmp.c_str()));

    // this will only evaluated if the request belongs to a location to avoid segfault
    if(this->request_->getLocation()) {
    tmp = "UPLOAD_TMP_DIR=" + this->request_->getLocation()->getUploadDir();
    this->env_.push_back(strdup(tmp.c_str()));
    }

    //for now this is hardcoded
    this->env_.push_back(strdup("REDIRECT_STATUS=200"));
    this->env_.push_back(NULL);
  } catch (...) {
    std::cerr << "bad request goes here " << std::endl;
  }
}

void Cgi::prepareArgs() {
  this->args_.push_back(strdup(this->request_->getRequestCgi().c_str()));
  this->args_.push_back(strdup(this->request_->getRequestedFileFullPath().c_str()));
  this->args_.push_back(NULL);
}

/*
* getters
*/

int Cgi::getResponseFileDescriptor() { return this->request_fd_; }

std::string Cgi::getResponseFileName() { return this->response_file_name_; }

bool Cgi::isFinished() {
  return this->finished_ != 0;
}