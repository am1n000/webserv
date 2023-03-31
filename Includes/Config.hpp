/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 17:49:09 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/31 00:25:20 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  CONFIG_HPP
#define  CONFIG_HPP

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>
#include "Server.hpp"
#include "Tockenizer.hpp"

//this header for dev only
#include "../dev/dev.hpp"
#include "helpers.hpp"

// to included config files i implemented this class so every time when i found an included file i will parse it
class ConfigFile {
  private:
    std::string stream_;
    std::ifstream file_;
    Tockenizer *tok_;  
public:
  ConfigFile();
  ConfigFile(const std::string& path);
  ~ConfigFile();

  void readFile();
  void close();
  Tockenizer* tockenizer();
};



class Config {
  public:
    typedef std::size_t size_type;

  private:
    Config();
    Config(const std::string& path);
    std::string path_;
    std::string buffer_;
    std::ifstream*  file_;
    std::streamsize file_size_;
    Tockenizer *tockenizer_;
    std::vector<Server*> servers_;
    static Config* object_;
    std::map<std::string, std::string> mime_types_;

  public:
    static Config* boot();
    static Config* boot(const std::string& path);
    static Config* get();
    static void cleenup();
    std::string read();
    void setFileSize();
    void  closeFile();
    void parse();
    void parseFile(const std::string& path);
    void parseMimeTypes(const std::string& scope);
    std::string getConfigBuffer() const;
    void pushServer(const std::string& serverString);
    std::vector<Server*> getServers();
    std::map<std::string, std::string> getMimeTypes();
    ~Config();
  /*
  * test method
  */
  void test();
};

#endif
