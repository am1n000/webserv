/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:57:48 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/11 02:27:03 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Cgi.hpp"
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/_types/_size_t.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "../dev/dev.hpp"

#define root "/Users/hchakoub/cursus/webserv/ressources"
#define cgi_path "/Users/hchakoub/.local/bin/php-cgi"

// Cgi::Cgi(): cgi_path_("/Users/hchakoub/.local/bin/php-cgi") {}

Cgi::Cgi(Request& request): request_(request) {
  // std::cout << "from cgi constructor" << std::endl;
  // std::cout << request.getRequestUri() << std::endl;
}

Cgi::~Cgi() {}

void Cgi::openFile(const std::string &path) {
  if(this->file_.is_open())
    this->file_.close();
  try {
    this->file_.open(path, std::fstream::out);
  } catch(...) {
    // printing error for now as placeholder
    std::cerr << "Error openning body file in cgi" << std::endl;
  }
}

void Cgi::appendStream(const char *buffer, size_t buffer_size) {
  if(!this->file_.is_open())
    throw std::runtime_error("file is not oppend");
  this->file_.write(buffer, buffer_size);
}

void printheaders(std::vector<char *> v) {

  for(size_t i = 0; i < v.size() - 1; i++)
    std::cout << v[i] << std::endl;
}

void Cgi::testCgi() {
  
  this->prepareEnv();
  printheaders(this->env_);
  int pid = fork();
  if (pid < 0) {
    std::cerr << "Fork error" << std::endl;
    exit(1);
  }

  if(pid == 0) {

    int fd = open("/Users/hchakoub/cursus/webserv/ressources/uploads/tmp/request.out", O_RDONLY);
    int resfd = open("/Users/hchakoub/cursus/webserv/ressources/uploads/tmp/response.out", O_RDWR | O_CREAT);
    if(fd < 0 || resfd < 0) {
      std::cerr << "error openning input file" << std::endl;
      exit(1);
    }

    std::vector<char *> args;
    args.push_back(const_cast<char *>("/Users/hchakoub/.local/bin/php-cgi"));
    // args.push_back(const_cast<char *>("/Users/hchakoub/cursus/webserv/ressources/post/data.php"));

      std::string sfn = root;
    sfn += request_.getRequestUri();
    args.push_back(const_cast<char *>(sfn.data()));
    args.push_back(NULL);
    dup2(fd, 0);
    // dup2(resfd, 1);
    if (execve(args[0], &args[0], &env_[0])) {
    std::cerr << "execve failed" << std::endl;
      std::cerr << strerror(errno) << std::endl;
      exit(1);
    }
  }
  wait(NULL);
}


/*
* prepare cgi
*/

void Cgi::prepareEnv() {
  // this->request_.test();
  try {
    std::string tmp = "CONTENT_LENGTH=" + request_.getHeaders().find("Content-Length")->second;
    this->env_.push_back(strdup(tmp.data()));
    tmp = "CONTENT_TYPE=" + request_.getHeaders().find("Content-Type")->second;
    this->env_.push_back(strdup(tmp.data()));
    tmp = "REQUEST_METHOD=" + Settings::methodString(request_.getRequestMethod());
    this->env_.push_back(strdup(tmp.data()));
    tmp = "SCRIPT_FILENAME=";
    tmp += root + request_.getRequestUri();
    this->env_.push_back(strdup(tmp.data()));

    //for now this is hardcoded
    this->env_.push_back(const_cast<char *>("REDIRECT_STATUS=200"));
    this->env_.push_back(NULL);
  } catch (...) {
    std::cerr << "bad request goes here " << std::endl;
  }
}

void Cgi::prepareArgs() {

}
