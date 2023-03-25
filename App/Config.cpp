/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/09 14:41:58 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/21 22:04:52 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Config.hpp"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include "../Includes/Server.hpp"

Config::Config(): path_("webserv.conf") {
  try {
  this->file_ = new std::ifstream(this->path_);
  this->setFileSize();
  this->read();
  this->tockenizer_ = new Tockenizer(this->buffer_);
  } catch (...) {
    std::cerr << "Config file could not be oppened"  << std::endl;
    exit(1);
  }
}

Config::Config(const std::string &path): path_(path) {
  try {
  this->file_ = new std::ifstream(this->path_);
  this->setFileSize();
  this->read();
  } catch (...) {
    std::cerr << "Config file could not be oppened"  << std::endl;
    exit(1);
  }
}


std::string Config::read() {
  if(!this->file_->is_open())
    throw std::runtime_error("file is not openned");
  this->buffer_.resize(this->file_size_);
  this->file_->read(&this->buffer_[0], this->file_size_);
  return this->buffer_;
}


void Config::setFileSize() {
  if(!this->file_->is_open())
    throw std::runtime_error("file is not openned");
  this->file_->seekg(0, std::ios::end);
  this->file_size_ = this->file_->tellg();
  this->file_->seekg(0, std::ios::beg);
}

void   Config::closeFile() {
  this->file_->close();
}

void Config::parse() {
  std::string token;
  while(!tockenizer_->end()) {
    token = this->tockenizer_->getNextToken();
    if(token == "server")
      this->pushServer(this->tockenizer_->getNextScope());
  }
  dev::br();
  std::cout << this->servers_[0]->getRoot() << std::endl;
  std::cout << this->servers_[0]->getClienBodySizeLimit() << std::endl;
  std::cout << this->servers_[0]->getHost() << std::endl;
  std::cout << this->servers_[0]->getPort() << std::endl;
  // std::cout << this->servers_[1]->getRoot() << std::endl;
  dev::br();
  std::cout << this->servers_[1]->getRoot() << std::endl;
  std::cout << this->servers_[1]->getClienBodySizeLimit() << std::endl;
  std::cout << this->servers_[1]->getHost() << std::endl;
  std::cout << this->servers_[1]->getPort() << std::endl;
}

void Config::pushServer(const std::string& serverString) {
  Server* server = new Server(serverString);
  servers_.push_back(server);
}

std::string Config::getConfigBuffer() const {
  return this->buffer_;
}

Config::~Config() {
 delete this->file_;
}
