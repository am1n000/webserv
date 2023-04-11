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
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/_types/_size_t.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

Cgi::Cgi(): cgi_path_("/Users/hchakoub/.local/bin/php-cgi") {}

Cgi::Cgi(const std::string& path) {
  try {

    this->file_.open(path, std::fstream::out);
  } catch(...) {
    // printing error for now as placeholder
    std::cerr << "Error openning body file in cgi" << std::endl;
  }
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

void Cgi::testCgi(std::string file, std::string cl) {
  
  // (void)cl;
  int pid = fork();
  if (pid < 0)
    std::cerr << "Fork error" << std::endl;

  if(pid == 0) {

    int fd = open("/Users/hchakoub/cursus/webserv/ressources/uploads/tmp/request.out", O_RDONLY);
    if(fd < 0)
      std::cerr << "error openning input file" << std::endl;
    std::vector<char *> env;
    std::vector<char *> args;
    args.push_back(const_cast<char *>("/Users/hchakoub/.local/bin/php-cgi"));
    args.push_back(const_cast<char *>("/Users/hchakoub/cursus/webserv/ressources/uploads/upload.php"));
    // args.push_back(const_cast<char *>("/Users/hchakoub/cursus/webserv/ressources/uploads/upload.php"));
    args.push_back(NULL);
    env.push_back(const_cast<char *>("REQUEST_METHOD=POST"));
    env.push_back(const_cast<char *>("REDIRECT_STATUS=200"));

    struct stat st;
    fstat(fd, &st);
    // std::string content_length = "CONTENT_LENGTH=" + std::to_string(st.st_size);
    std::string content_length = "CONTENT_LENGTH=" + cl;
    env.push_back(const_cast<char *>(content_length.c_str()));
    env.push_back(const_cast<char *>(std::string("CONTENT_TYPE=" + file).c_str()));
    env.push_back(const_cast<char *>("SCRIPT_FILENAME=./ressources/uploads/upload.php"));
    env.push_back(NULL);

    dup2(fd, 0);
    if (execve(this->cgi_path_.data(), &args[0], &env[0]))
    std::cerr << "execve failed" << std::endl;
  }
  wait(NULL);
}
