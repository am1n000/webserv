/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 15:04:02 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/13 01:35:12 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

Server::Server() {}

Server::~Server() {}

void Server::setRoot(const std::string &val) {
  this->root_ = val;
}

void Server::setClientBodySize(const std::string &val){
  try {
  this->client_body_size_limit_ = std::stoi(val);
    std::cout << this->client_body_size_limit_ << std::endl;
  } catch (...) {
    std::cerr << "client body size limit is invaid" << std::endl;
    exit(1);
  }
}

void Server::setIndex(const std::string &val){
  char* token;
  token = std::strtok(const_cast<char*>(val.data()), " ");
  while (token != NULL) {
    this->indexs_.push_back(token);
    token = std::strtok(NULL, " ");
  }
}

void Server::setServerName(const std::string &val){
  std::cout << "set  server name has been called" << std::endl;
(void) val;
}
void Server::setListen(const std::string &val){
  std::size_t i = val.find(":");
  if (i == std::string::npos) {
    this->host_ = "0.0.0.0";
    this->port_ = val;
  } else {
    this-> host_ = val.substr(0, i);
    this->port_ = val.substr(i + 1, val.length());
  }
}
void Server::setErrorPage(const std::string &val){
  std::cout << "set error page has been called" << std::endl;
(void) val;
}


void Server::setMemebers() {
  this->members_.insert(std::make_pair("root", &Server::setRoot));
  this->members_.insert(std::make_pair("index", &Server::setIndex));
  this->members_.insert(std::make_pair("server_name", &Server::setServerName));
  this->members_.insert(std::make_pair("listen", &Server::setListen));
  this->members_.insert(std::make_pair("error_page", &Server::setErrorPage));
  this->members_.insert(std::make_pair("client_body_limit", &Server::setClientBodySize));
}


// only for testing
void Server::test() {
  for(std::size_t i = 0; i < this->indexs_.size(); i++)
    std::cout << this->indexs_[i] << std::endl;
}

void Server::setProp(const std::string &prop, const std::string &val) {
  (this->*members_[prop])(val);
}
