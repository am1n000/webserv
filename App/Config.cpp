/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/09 14:41:58 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/11 18:39:20 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Config.hpp"
#include <string>
#include <iostream>

Config::Config(): path_("webserv.conf"), file_(new std::ifstream(this->path_)){
  this->setFileSize();
}

Config::Config(const std::string &path): path_(path), file_(new std::ifstream(this->path_)), file_size_(this->file_->tellg()){}


std::string Config::read() {
  std::cout << this->file_size_ << std::endl;
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

Config::~Config() {
 delete this->file_;
}
