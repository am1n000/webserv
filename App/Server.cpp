/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 15:04:02 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/21 22:37:54 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include "../Includes/helpers.hpp"
//for dev only
#include "../dev/dev.hpp"


Server::Server() {
  this->setMemebers();
}

Server::Server(const std::string& serverString): server_string_(serverString) {
  this->setMemebers();
  this->tockenizer_ = new Tockenizer(serverString.substr(1, serverString.length() - 2));
  this->parseServer();
}

Server::~Server() {}

void Server::setRoot(const std::string &val) {
  this->root_ = val;
}

void Server::setClientBodySize(const std::string &val){
  try {
  this->client_body_size_limit_ = std::stoi(val);
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
  std::cout << "set listen trigred" << std::endl;
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
  this->members_.insert(std::make_pair("location", &Server::pushLocation));
}


// only for testing
void Server::test() {
  // for(std::size_t i = 0; i < this->indexs_.size(); i++)
  //   std::cout << this->indexs_[i] << std::endl;
  this->setProp("root", "helloroot");
}
// end test

void  Server::pushLocation(const std::string& locationString) {
  (void) locationString;
}

void Server::setProp(const std::string &prop, const std::string &val) {
  (this->*members_[prop])(helpers::trim(val));
}

Location::Location() {};
Location::Location(const std::string& locationString) { 
  (void)locationString;
  std::cout << "location has been parsed" << std::endl;
};

void Server::parseServer() {
  std::string token;
  std::string val;

  while(!tockenizer_->end()) { 
    token = this->tockenizer_->getNextToken();
    val = this->tockenizer_->getLine();
    if (token.length() == 0 || val.length() == 0)
      break;
      // std::cout << "we got an error here " << std::endl;
    this->setProp(token, val);
  };
}

// getters 
std::string& Server::getRoot() {
  return this->root_;
}
std::string &Server::getHost() { return this->host_; }
std::string &Server::getPort() { return this->port_;}
std::string &Server::getServerName() {return this->server_name_;}
std::string &Server::getErrorPage() {return this->error_page_;}
std::size_t &Server::getClienBodySizeLimit() { return this->client_body_size_limit_;}
std::vector<std::string> &Server::getIndexes() { return this->indexs_;}
