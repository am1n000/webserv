/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/09 14:41:16 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/09 14:43:14 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Request.hpp"

Request::Request(): _method() {}

Request::Request(const Request& request) {}

Request::Request(const std::string requestString)
    : _request_string(requestString) {
  this->_parseRequest();
  // Cgi cgi(this->_header, this->_method);
  // ft::br();
  // cgi.process();

}

void Request::_parseRequest() {

  std::istringstream stream(this->_request_string);
  std::string line;
  std::size_t accurance;

  // getting first line [method, request uri , http version]
  std::getline(stream, line);
  this->setMethod(line);

  // parsing header
  while(std::getline(stream, line)) {
    accurance = line.find(":");
    if (accurance != std::string::npos) {
      this->_header_string += line + "\n";
      std::string key(line.substr(0, accurance));
      std::string value(ft::trim(line.substr(accurance + 1), ' '));
      this->_header.insert(std::make_pair(key, value));
    } else 
      break;
  }
  
  // parsing body
  while(std::getline(stream, line)) {
    this->_body_string += line + "\r\n";
  }
}

std::string Request::getMethod() const {
  return this->_method;
}

void Request::_parseHeader() {

}

void Request::setMethod(const std::string method) {
  char *token = const_cast<char *>(method.data());
  this->_method = std::strtok(token, " ");
  this->_uri = std::strtok(NULL, " ");
  this->_http_version = std::strtok(NULL, " ");
}

Request::~Request() {
  std::cout << "request destroyed" <<std::endl;
}
