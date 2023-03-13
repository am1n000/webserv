/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 15:03:44 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/12 19:18:39 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <map>

class Location {
private:
  std::vector<std::string> allowed_methods_;
  std::string redirection_;
  std::string root_;
  bool auto_index_;
  std::string index_;
  std::map<std::string, std::string> cgis_;
  std::string upload_dir_;

public:
  Location();

  void setIndex();
  void setCgi();
  void setRedirection();
  void setAutoIndex();

  std::string getIndex() const;
  std::string getCgi() const;
  std::string getRedirection() const;
  bool getAutoIndex() const;
};

class Server {
public:
  typedef void (Server::*memberPointer)(const std::string&);

  private:
    std::string root_;
    std::string host_;
    std::string port_;
    std::string server_name_;
    std::string error_page_;
    std::size_t client_body_size_limit_;
    std::vector<std::string> indexs_;
    std::vector<Location> locations_;
    std::map<std::string, memberPointer> members_;
public:
  Server();

  void setRoot(const std::string& val);
  void setHost(const std::string& val);
  void setPort(const std::string& val);
  void setServerName(const std::string& val);
  void setErrorPage(const std::string& val);
  void setClientBodySize(const std::string& val);
  void setIndex(const std::string& index);
  void setListen(const std::string& val);
  void pushLocation(const Location& location);
  void setMemebers();
  // only for testing 
  void test();

  void setProp(const std::string& prop, const std::string& val);
  ~Server();
};
