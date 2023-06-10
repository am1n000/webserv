/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 15:04:02 by hchakoub          #+#    #+#             */
/*   Updated: 2023/06/10 14:55:53 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Server.hpp"
#include "../Includes/Settings.hpp"
#include "../Includes/helpers.hpp"
// for dev only
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <utility>

Server::Server() { this->setMembers(); }

Server::Server(const std::string &serverString) : server_string_(serverString), client_body_size_limit_(-1) {
  this->setMembers();
  this->tockenizer_ =
      new Tockenizer(serverString.substr(1, serverString.length() - 2));
  this->parseServer();
}

Server::~Server() {}

/*
 * server setters
 */
void Server::setRoot(const std::string &val) { 
  this->root_ = val; 
  if(this->root_.empty())
    throw BadConfigException("emty value for root");
  if(this->root_[this->root_.length() - 1] == '/')
    this->root_.erase(this->root_.end() - 1);

  // check if directory exists
  DIR* rootdir;
  rootdir = opendir(this->root_.c_str());
  if(!rootdir)
    throw BadConfigException("invalid root directory");
}

void Server::setClientBodySize(const std::string &val) {
  try {
    this->client_body_size_limit_ = std::atoi(val.data());
  } catch (...) {
    throw BadConfigException();
  }
}

void Server::setIndex(const std::string &val) {
  char *token;
  token = std::strtok(const_cast<char *>(val.data()), " ");
  while (token != NULL) {
    this->indexs_.push_back(token);
    token = std::strtok(NULL, " ");
  }
}

void Server::setServerName(const std::string &val) { this->server_name_ = val; }

void Server::setListen(const std::string &val) {
  std::size_t i = val.find(":");
  if (i == std::string::npos) {
    this->host_ = "0.0.0.0";
    this->port_ = val;
  } else {
    this->host_ = val.substr(0, i);
    this->port_ = val.substr(i + 1, val.length());
  }
}

void Server::setErrorPages(const std::string &val) { 

  Tockenizer tok(val);
  std::string code = tok.getNextToken();
  std::string path = helpers::trim(tok.getNoneEmptyLine());
  if(code.empty() || path.empty())
    throw BadConfigException("invalid error page");
  this->error_pages_.insert(std::make_pair(std::string(code), std::string(path)));
}

void Server::setMembers() {
  this->members_.insert(std::make_pair("root", &Server::setRoot));
  this->members_.insert(std::make_pair("index", &Server::setIndex));
  this->members_.insert(std::make_pair("server_name", &Server::setServerName));
  this->members_.insert(std::make_pair("listen", &Server::setListen));
  this->members_.insert(std::make_pair("error_page", &Server::setErrorPages));
  this->members_.insert(
      std::make_pair("client_body_limit", &Server::setClientBodySize));
  this->members_.insert(std::make_pair("location", &Server::pushLocation));
}

// only for testing
void Server::test() { this->setProp("root", "helloroot"); }
// end test

void Server::pushLocation(const std::string &locationString) {
  std::string ls = locationString;
  std::string slash = "/";
  if(ls.empty() || ls[0] == '{')
    throw BadConfigException("invalid location value");
  if (ls[0] != '/') {
    ls = slash + ls;
  }
  if(ls[ls.length() - 1] == '/' && ls.length() > 1)
    ls.erase(ls.end() - 1);
  Location *location = new Location(ls, this->tockenizer_->getNextScope());
  this->locations_.insert(std::make_pair(ls, location));
}

void Server::setProp(const std::string &prop, const std::string &val) {
  if (Server::members_.find(prop) == Server::members_.end())
    throw BadConfigException("invalid directive, " + prop);
  if (val.length() == 0)
    throw BadConfigException("Empty value for: " + prop);
  (this->*members_[prop])(helpers::trim(val));
}

void Server::parseServer() {
  std::string token;
  std::string val;

  while (!tockenizer_->end()) {
    token = this->tockenizer_->getNextToken();
    if (token.length() == 0)
      break;
    // if (!Server::inDictinary(token))
    // 	throw std::runtime_error("invalid directive " + token);

    if (token == "location")
      val = this->tockenizer_->getNextToken();
    else
      val = this->tockenizer_->getLine();
    this->setProp(token, helpers::trim(val));
  };
}

/*
 * getters
 */

std::string &Server::getRoot() { return this->root_; }
std::string &Server::getHost() { return this->host_; }
std::string &Server::getPort() { return this->port_; }
std::string &Server::getServerName() { return this->server_name_; }
std::map<std::string, std::string> &Server::getErrorPages() { return this->error_pages_; }
std::size_t &Server::getClienBodySizeLimit() { return this->client_body_size_limit_; }

std::vector<std::string> &Server::getIndexes() { return this->indexs_; }

const char *Server::pdictionary_[] = {"root",        "index",
                                      "server_name", "listen",
                                      "error_page",  "client_body_limit",
                                      "location",    NULL};

std::vector<std::string> Server::dictionary_;

void Server::setDictionary() {
  int i(0);
  while (pdictionary_[i]) {
    dictionary_.push_back(pdictionary_[i]);
    i++;
  }
}

bool Server::inDictinary(const std::string &token) {
  for (std::vector<std::string>::iterator it = Server::dictionary_.begin();
       it != Server::dictionary_.end(); it++) {
    if (*it == token)
      return true;
  }
  return false;
}

/*
 * Location members
 *
 */

Location::Location() { this->setMembers(); };

Location::Location(const std::string &location,
                   const std::string &locationScope)
    : auto_index_(false) {
  this->location_ = location;
  this->tockenizer_ = new Tockenizer(locationScope);
  this->setMembers();
  this->parse();
};

Location::~Location() {}

void Location::parse() {
  std::string key;
  std::string value;
  while (!this->tockenizer_->end()) {
    key = this->tockenizer_->getNextToken();
    value = this->tockenizer_->getLine();
    if ((key.length() == 0) ^ (value.length() == 0))
      throw BadConfigException("invalid value for " + key);
    if (key.length() != 0 && value.length() != 0)
      this->setProp(key, helpers::trim(value));
  }
}

std::map<std::string, Location::memberPointer> Location::location_members_;

void Location::setMembers() {
  Location::location_members_.insert(
      std::make_pair("root", &Location::setRoot));
  Location::location_members_.insert(
      std::make_pair("index", &Location::setIndex));
  Location::location_members_.insert(
      std::make_pair("allowed_methods", &Location::setAllowedMethods));
  Location::location_members_.insert(std::make_pair("cgi", &Location::setCgi));
  Location::location_members_.insert(
      std::make_pair("redirection", &Location::setRedirection));
  Location::location_members_.insert(
      std::make_pair("auto_indexing", &Location::setAutoIndex));
  Location::location_members_.insert(
      std::make_pair("upload_dir", &Location::setUploadDir));
}

/*
 * location setters
 */
void Location::setIndex(const std::string &val) {
  Tockenizer tok(val);
  while (!tok.end()) {
    this->indexs_.push_back(helpers::trim(tok.getNextToken()));
  }
}

void Location::setCgi(const std::string &val) {
  Tockenizer tok(val);
  std::string extention;
  std::string cgi;
  extention = tok.getNextToken();
  cgi = tok.getNextToken();
  if (extention.length() == 0 || cgi.length() == 0)
    throw std::runtime_error("bad cgi directive");
  this->cgis_.insert(std::make_pair(extention, cgi));
}

void Location::setRedirection(const std::string &val) {
  this->redirection_ = val;
}

void Location::setAutoIndex(const std::string &val) {
  if (val != "on" && val != "off")
    throw std::runtime_error("invalid value for auto_indexing");
  this->auto_index_ = val == "on" ? true : false;
}

void Location::setAllowedMethods(const std::string &val) {
  Tockenizer tok = Tockenizer(val);
  while (!tok.end()) {
    Request_Method_e method =
        Settings::get()->indexOfRequestMethod(tok.getNextToken());
    this->allowed_methods_.push_back(method);
  }
}
void Location::setRoot(const std::string &val) {
  Tockenizer tok = Tockenizer(val);
  this->root_ = helpers::trim(tok.getLine());
  if(this->root_.empty() || !tok.end())
    throw BadConfigException("invalid value for root");
  if(this->root_[this->root_.length() - 1] == '/') 
    this->root_.erase(this->root_.end() - 1);

  // check if directory exists
  DIR* rootdir;
  rootdir = opendir(this->root_.c_str());
  if(!rootdir)
    throw BadConfigException("invalid root directory");
}

void Location::setProp(const std::string &prop, const std::string &val) {
  if (this->Location::location_members_.find(prop) ==
      Location::location_members_.end())
    throw BadConfigException("invalid directive, " + prop);
  if (val.length() == 0)
    throw BadConfigException("Empty value for: " + prop);
  (this->*Location::location_members_[prop])(val);
}

void Location::setUploadDir(const std::string &val) { this->upload_dir_ = val; }

/*
 * location getters
 */

std::map<std::string, std::string> &Location::getCgis() { return this->cgis_; }
std::string Location::getRoot() const { return this->root_; }
std::string Location::getUploadDir() const { return this->upload_dir_; }
bool Location::getAutoIndex() const { return this->auto_index_; }
std::vector<Request_Method_e> &Location::getAllowedMethods() { return this->allowed_methods_;  }
std::vector<std::string> &Location::getIndex() { return (this->indexs_); }
const std::string& Location::getRedirection() const { return this->redirection_; }
/*
 * location getters end
 */
// socket related functions

int Server::createSocket() {
  int sock;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::cerr << "error: socket" << std::endl;
    return (-1);
  }
  fcntl(sock, F_SETFL, O_NONBLOCK);
  return (sock);
}

bool Server::bindSocket(int sock) {
  this->_hostAddrlen = sizeof(this->_hostAddr);
  memset(&this->_hostAddr, 0, sizeof(this->_hostAddr));
  this->_hostAddr.sin_family = AF_INET;
  this->_hostAddr.sin_port = htons(std::atoi(this->getPort().data()));
  this->_hostAddr.sin_addr.s_addr = inet_addr(this->getHost().c_str());

  int opt = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::cerr << "error: setsockopt" << std::endl;
    return (true);
  }
  opt = 1;
#ifdef __APPLE__
  if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)) < 0) {
    std::cerr << "error: setsockopt" << std::endl;
    return (true);
  }
#endif
  if (bind(sock, reinterpret_cast<struct sockaddr *>(&this->_hostAddr),
           this->_hostAddrlen) != 0)
    return (true);
  return (false);
}

bool Server::listenToConnections(int sock) {
  if (listen(sock, SOMAXCONN) == -1) {
    std::cerr << "error: listen" << std::endl;
    return (true);
  }
  return (false);
}
int &Server::getHostAddrlen() { return (this->_hostAddrlen); }

struct sockaddr_in &Server::getHostAddr() { return (this->_hostAddr); }
