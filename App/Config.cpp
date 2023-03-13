/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/09 14:41:58 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/13 12:27:14 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Config.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include "../Includes/Server.hpp"

Config::Config(): path_("webserv.conf"), file_(new std::ifstream(this->path_)){
  this->setFileSize();
}

Config::Config(const std::string &path): path_(path), file_(new std::ifstream(this->path_)), file_size_(this->file_->tellg()){}


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
  token = std::strtok(&this->buffer_[0], " ");
  std::cout << token ;
  token = std::strtok(NULL, "\n");
  std::cout << token ;
  token = std::strtok(NULL, "\n");
  // token = std::strtok(NULL, "}");
  std::cout << token << std::endl;
}

// std::string Config::getServerString(size_type start) {
// }

void Config::parseServer() {
  // Server *server = new Server();
}


std::string Config::getConfigBuffer() const {
  return this->buffer_;
}

Config::~Config() {
 delete this->file_;
}
