/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/09 14:41:58 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/01 23:47:50 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Config.hpp"
#include "../Includes/Server.hpp"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/_types/_size_t.h>
#include <utility>
#include <vector>

Config::Config() : path_("webserv.conf") {
  try {
    this->file_ = new std::ifstream(this->path_);
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
    this->file_ = new std::ifstream(this->path_);
    this->setFileSize();
    this->read();
  } catch (...) {
    std::cerr << "Config file could not be oppened" << std::endl;
    exit(1);
  }
}

std::string Config::read() {
  if (!this->file_->is_open())
    throw std::runtime_error("can't read config, file is not openned");
  this->buffer_.resize(this->file_size_);
  this->file_->read(&this->buffer_[0], this->file_size_);
  return this->buffer_;
}

void Config::setFileSize() {
  if (!this->file_->is_open())
    throw std::runtime_error("can't read config, file is not openned");
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
    else if (token == "include") {
      this->parseFile(this->tockenizer_->getNoneEmptyLine());
    }
    // std::cout << "there is an include directive" <<
    // this->tockenizer_->getNoneEmptyLine() << std::endl;
  }
  // dev::br();
  // std::cout << this->servers_[0]->getRoot() << std::endl;
  // std::cout << this->servers_[0]->getClienBodySizeLimit() << std::endl;
  // std::cout << this->servers_[0]->getHost() << std::endl;
  // std::cout << this->servers_[0]->getPort() << std::endl;
  // // std::cout << this->servers_[1]->getRoot() << std::endl;
  // dev::br();
  // std::cout << this->servers_[1]->getRoot() << std::endl;
  // std::cout << this->servers_[1]->getClienBodySizeLimit() << std::endl;
  // std::cout << this->servers_[1]->getHost() << std::endl;
  // std::cout << this->servers_[1]->getPort() << std::endl;
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
    // std::cout << file.tockenizer()->getNextScope() << std::endl;
  }
}

void Config::parseMimeTypes(const std::string &scope) {
  Tockenizer tok(scope);
  std::string key;
  std::string value;

  while (!tok.end()) {
    value = helpers::trim(tok.getNextToken());
    key = helpers::trim(tok.getNextToken(';'));
    if (key.length() != 0 && value.length() != 0)
      this->mime_types_.insert(std::make_pair(key, value));
    if ((key.length() == 0) ^ (value.length() == 0))
      throw std::runtime_error("error in mime types file");
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
  return Config::object_;
}

Config *Config::boot(const std::string &path) {
  if (Config::object_ != NULL)
    return Config::object_;
  Config::object_ = new Config(path);
  return Config::object_;
}

std::vector<Server *> Config::getServers() { return this->servers_; }

std::map<std::string, std::string>& Config::getMimeTypes() {
  return this->mime_types_;
}

Config *Config::get() { return Config::object_; }

/*
 * tests
 */
void Config::test() {
  for(std::map<std::string, std::string>::iterator it = this->mime_types_.begin(); it != this->mime_types_.end(); it++)
    std::cout << it->first << "  " << it->second << std::endl;
}

void Config::cleanup() {
  // this is the method responsable for cleening the object i will let it empty
  // for now
}

/* --------------------------------------------------------------------------------------
 *
 *  section for parse file class
 *
 * --------------------------------------------------------------------------------------
 */

ConfigFile::ConfigFile() {
  throw std::runtime_error("file path need to pe specified");
}

ConfigFile::ConfigFile(const std::string &path) {
  try {
    file_.open(path);
    try {
      this->readFile();
      this->close();
      this->tok_ = new Tockenizer(this->stream_);
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << std::endl;
    }
  } catch (...) {
    std::cerr << "file: " << path << " can not be oppened" << std::endl;
  }
}

void ConfigFile::readFile() {
  size_t stream_size;
  // checking file is oppened
  if (!this->file_.is_open())
    throw std::runtime_error("can't read file size, file is not openned");

  // getting the byte size of the file
  this->file_.seekg(0, std::ios::end);
  stream_size = this->file_.tellg();
  this->file_.seekg(0, std::ios::beg);

  char buffer[stream_size];
  this->file_.read(buffer, stream_size);
  this->stream_ = buffer;
}

void ConfigFile::close() { this->file_.close(); }

Tockenizer *ConfigFile::tockenizer() { return this->tok_; }

ConfigFile::~ConfigFile() {}
