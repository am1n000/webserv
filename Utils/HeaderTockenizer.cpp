/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderTockenizer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 11:18:10 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/16 14:25:54 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/HeaderTockernizer.hpp"
#include "../Includes/Exceptions.hpp"
#include "../Includes/helpers.hpp"
#include <iterator>
#include <utility>

HeaderTockernizer::HeaderTockernizer(): Tockenizer() {}
HeaderTockernizer::HeaderTockernizer(HeaderTockernizer::const_reference data): Tockenizer(data) {
  size_type pos = this->data_.find("\r\n\r\n");
  if(pos == std::string::npos)
    throw BadRequestException();
  this->data_.erase(pos);
}
HeaderTockernizer::~HeaderTockernizer() {}

HeaderTockernizer::data_type HeaderTockernizer::getKey() {
  size_type i = this->current_;
  while(std::isalnum(this->data_[this->current_]) && this->data_[this->current_] != ':'){
    this->current_++;
  }
  // std::cout << this->data_[this->current_] << std::endl;
  // if(this->data_[this->current_] != ':')
  //   throw BadRequestException();
  this->current_++;
  return this->data_.substr(i, this->current_);
}

// to be removed lather
HeaderTockernizer::data_type HeaderTockernizer::getHeaders() {
  size_type size;
 size_type pos = this->data_.find("\r\n\r\n"); 
  if (pos == std::string::npos)
    throw std::runtime_error("header not found");
 size = pos - this->current_; 
  std::string header = this->data_.substr(this->current_, size);
  this->current_ += size + 4;
  return header;
}

HeaderTockernizer::data_type HeaderTockernizer::getNoneEmptyLine() {
  data_type line;
  size_type pos = this->data_.find("\r\n", this->current_);
  if(pos != std::string::npos) {
    line = this->data_.substr(this->current_, pos - this->current_);
    this->current_ = pos + 2;
  }
  if(this->current_ >= this->data_.length())
    this->end_ = true;
  // if(line.length() == 0)
  //   throw BadRequestException();
  return line;
}

std::pair<std::string, std::string> HeaderTockernizer::getNextHeader() {
  std::string key;
  std::string value;
  std::string line;
  size_type pos;

  line = this->getNoneEmptyLine();
  std::cout << "line: " << line << std::endl;
  pos = line.find(":");
  if(pos == std::string::npos)
    throw BadRequestException();
  key = line.substr(0, pos);
  value = helpers::trim(line.substr(pos + 1));
  // if(value.length() || key.length() == 0)
  //   throw BadRequestException();
  return std::make_pair(key, value);
}
