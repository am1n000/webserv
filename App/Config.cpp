/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/09 14:41:58 by hchakoub          #+#    #+#             */
/*   Updated: 2023/06/06 09:34:55 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Config.hpp"
#include "../Includes/Server.hpp"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

Config::Config() : path_("webserv.conf") {
  try {
    this->file_ = new std::ifstream(this->path_.c_str());
    this->setFileSize();
    this->read();
    this->tockenizer_ = new Tockenizer(this->buffer_);
  } catch (...) {
    std::cerr << "Config file could not be oppened" << std::endl;
    exit(1);
  }
}

Config::Config(const std::string &path) : path_(path) {
  try {
    this->file_ = new std::ifstream(this->path_.c_str());
    this->setFileSize();
    this->read();
    this->tockenizer_ = new Tockenizer(this->buffer_);
  } catch (...) {
    std::cerr << "Config file could not be oppened" << std::endl;
    exit(1);
  }
}

std::string Config::read() {
  if (!this->file_->is_open())
    throw BadConfigException("can not opent a config file" + this->path_);
  this->buffer_.resize(this->file_size_);
  this->file_->read(&this->buffer_[0], this->file_size_);
  helpers::removeComments(this->buffer_);
  return this->buffer_;
}

void Config::setFileSize() {
  if (!this->file_->is_open())
    throw BadConfigException("can not opent a config file" + this->path_);
  this->file_->seekg(0, std::ios::end);
  this->file_size_ = this->file_->tellg();
  this->file_->seekg(0, std::ios::beg);
}

void Config::closeFile() { this->file_->close(); }

void Config::parse() {
  std::string token;
  while (!tockenizer_->end()) {
    token = this->tockenizer_->getNextToken();
    if (token == "server")
      this->pushServer(this->tockenizer_->getNextScope());
    else if (token == "include")
      this->parseFile(this->tockenizer_->getNoneEmptyLine());
    else {
      if (token.length() > 1){
        throw BadConfigException("invalid directive");
      }
    }
  }
}

void Config::pushServer(const std::string &serverString) {
  Server *server = new Server(serverString);
  servers_.push_back(server);
}

std::string Config::getConfigBuffer() const { return this->buffer_; }

Config::~Config() { delete this->file_; }

/*
 * parsers
 */

void Config::parseFile(const std::string &path) {
  ConfigFile file(path);
  std::string token;
  while (!file.tockenizer()->end()) {
    token = file.tockenizer()->getNextToken();
    if (token == "types")
      this->parseMimeTypes(file.tockenizer()->getNextScope());
    else if (token == "server")
      this->pushServer(file.tockenizer()->getNextScope());
    else if (token == "include")
      this->parseFile(file.tockenizer()->getNoneEmptyLine());
    else {
      if (token.length() > 0 && std::isalnum(token[0])){
        throw BadConfigException("invalid directive");
      }
    }
  }
}

void Config::parseMimeTypes(const std::string &scope) {
  Tockenizer tok(scope);
  std::string key;
  std::string value;

  while (!tok.end()) {
    value = helpers::trim(tok.getNextToken());
    key = helpers::trim(tok.getNoneEmptyLine());
    if (key.length() != 0 && value.length() != 0)
      this->mime_types_.insert(std::make_pair(key, value));
    if ((key.length() == 0) ^ (value.length() == 0))
      throw BadConfigException("Invalid mime types");
  }
}

/*
 * bootstrap config
 */

Config *Config::object_ = NULL;

Config *Config::boot() {
  if (Config::object_ != NULL)
    return Config::object_;
  Config::object_ = new Config();
  Config::object_->parse();
  Config::checkHealth(Config::object_);
  return Config::object_;
}

Config *Config::boot(const std::string path) {
  if (Config::object_ != NULL)
    return Config::object_;
  Config::object_ = new Config(path);
  Config::object_->parse();
  Config::checkHealth(Config::object_);
  return Config::object_;
}

std::vector<Server *> &Config::getServers() { return this->servers_; }

std::map<std::string, std::string> &Config::getMimeTypes() {
  return this->mime_types_;
}

Config *Config::get() { return Config::object_; }

/*
 * checkers
 */

void Config::checkHealth(Config *config) {
  if (config == NULL)
    throw BadConfigException();
  for (std::vector<Server *>::iterator it = config->servers_.begin();
       it != config->servers_.end(); it++) {
    if (!Config::isServerValid(*it))
      throw BadConfigException();
  }
}

bool Config::isServerValid(Server *server) {
  if (server->getPort().empty() || server->getRoot().empty())
    return false;
  return true;
}

void Config::cleanup() {
  // this is the method responsable for cleening the object i will let it empty
  // for now
  if(Config::object_->file_) {
    if(Config::object_->file_->is_open())
      Config::object_->file_->close();
    delete Config::object_->file_;
  }
  if(Config::object_->tockenizer_)
    delete Config::object_->tockenizer_;
  
  std::vector<Server*> & servers = Config::object_->servers_;

  for(size_type i = 0; i < servers.size(); i++)
    delete servers[i];
}

/* --------------------------------------------------------------------------------------
 *
 *  section for parse file class
 *
 * --------------------------------------------------------------------------------------
 */

ConfigFile::ConfigFile() {
  throw BadConfigException("file path need to pe specified");
}

ConfigFile::ConfigFile(const std::string &path) {
  try {
    file_.open(path.c_str());
    try {
      this->readFile();
      this->close();
      this->tok_ = new Tockenizer(this->stream_);
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << std::endl;
      throw BadConfigException();
    }
  } catch (...) {
    throw BadConfigException();
  }
}

void ConfigFile::readFile() {
  size_t stream_size;
  // checking file is oppened
  if (!this->file_.is_open())
    throw BadConfigException("can not opent a config file");

  // getting the byte size of the file
  this->file_.seekg(0, std::ios::end);
  stream_size = this->file_.tellg();
  this->file_.seekg(0, std::ios::beg);

  char buffer[stream_size];
  this->file_.read(buffer, stream_size);
  this->stream_ = buffer;
  helpers::removeComments(this->stream_);
}

void ConfigFile::close() { this->file_.close(); }

Tockenizer *ConfigFile::tockenizer() { return this->tok_; }

ConfigFile::~ConfigFile() {
  if(this->tok_)
    delete this->tok_;
  if(this->file_.is_open())
    this->file_.close();
}
