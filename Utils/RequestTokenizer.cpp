/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestTokenizer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 09:47:26 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/22 10:18:21 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/RequestTokenizer.hpp"
#include <cctype>
#include <utility>

RequestTokenizer::RequestTokenizer() {}

RequestTokenizer::RequestTokenizer(const std::string& data): Tockenizer(data) {}

RequestTokenizer::~RequestTokenizer() {}

std::string RequestTokenizer::getLine() {
  std::string line;
  size_type pos = this->data_.find("\r\n", this->current_);
  if(pos != std::string::npos) {
    if(pos == 0)
      return line;
    line = this->data_.substr(current_, pos - current_);
    this->current_ += line.length() + 2;
  }
  return line;
}

std::map<std::string, std::string> RequestTokenizer::parseHeaders() {
 
  std::string line;
  std::string key;
  std::string value;
  std::map<std::string, std::string> headers;
  size_type pos;

  line = this->getLine();
  while (line.length() > 0) {
    pos = line.find(":");
    if(pos == std::string::npos)
      throw BadRequestException();
    key = line.substr(0, pos);
    if(key.length() == 0 || !this->isKeyValid(key))
      throw BadRequestException();
    value = helpers::trim(line.substr(pos + 1));
    // here you should check the key
    headers.insert(std::make_pair(key, value));
    line = this->getLine();
  }
  return headers;
}

bool RequestTokenizer::isKeyValid(const std::string &key) {
  for(size_type i = 0; i < key.length(); i++) {
    if (!this->isAllowedChar_(key[i]))
      return false;
  }
  return true;
}

/*
* private methods
*/

bool RequestTokenizer::isAllowedChar_(const char c) {
  if(std::isalnum(c) || c == '_' || c == '-')
    return true;
  return false;
}
