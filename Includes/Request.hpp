/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:49:20 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/01 13:25:08 by hchakoub         ###   ########.fr       */
/*   Updated: 2023/05/01 16:54:49 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <fstream>
#include <sys/socket.h>
#include <iostream>
#include <map>
#include "../Includes/Tockenizer.hpp"
#include "Settings.hpp"
#include "../Includes/Exceptions/Exceptions.hpp"
#include "../Includes/helpers.hpp"

#define BUFFER_SIZE 4096
#define REQUEST_SEPARATOR "\r\n\r\n"
#define TMP_FILE_NAME "/Users/hchakoub/cursus/webserv/ressources/uploads/tmp/request.out"

// must be included on the response, i defined it here to avoid conflit
// also fstream header must be included on response header

// adding Server class header to avoid recursive inclusion
// adding classes header to avoid recursive inclusion
class Server;
class Location;
typedef  struct t_file {
  std::string  filename; 
std::string media;
} s_file;

// also must be included on the global header 
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
// #define  MAX_EVENTS 10

class Request {
public:
  typedef size_t size_type;
  typedef Request_Method_e method_type;

private:
  Tockenizer *tockenizer_;
  std::string request_string_;
  std::string header_string_;
  std::string body_string_;
  bool header_completed_;
  bool body_completed_;
  method_type request_method_;
  std::string request_uri_;
  std::string query_parameters_;
  std::string http_version_;
  std::map<std::string, std::string> request_headers_;
  size_type content_length;
  std::fstream *body_file_;
  size_type body_size_;
  std::string extention_;
  std::string filename_;
  Server *server_;
  //the file is temporary just to work with amine's code
  // to be changed lather
  s_file file_;

  /*
  * private methods
  */
  void parseMetadata_(const std::string& metadata);
  void pushHeaders_();
  void setExtention_();
  

public:
  Request();
  Request(size_type buffer_size);
  Request(char *buffer, size_type read_size, size_type buffer_size = BUFFER_SIZE);
  ~Request();

  size_type buffer_size;

/*
 * parsers
 */

  void parseHeader();

  /*
   * checkers
   */

  bool isHeaderCompleted();
  bool isBodyCompleted();
  bool isRequestCompleted();
  Location* matchLocation();

  /*
   * modifiers
   */

  int appendBuffer(char *buffer, size_type recieved_size);
  void appendBodyFile(const char *buffer, size_type buffer_size);

  /*
  * setters
  */

  void setMethod(const std::string& method);
  void setRequestUri(const std::string& uri);
  void setHttpVersion(const std::string& version);
  void setHeaderString();
  void setContentLength();
  void setServer(Server *server);

  /*
   * getters
   */

  size_type getContentLength() ;
  int getRequestMethod() const;
  // temoraty to be changed with the file struct
  s_file getFile();
  std::string getRequestString() const;
  std::string getBodyString() const;
  std::string getHeaderString() const;
  size_type getBodySize() const;
  std::map<std::string, std::string> &getHeaders();
  const std::string &getRequestUri();
  std::string getExtention() const;
  Server* getServer() const;


  /*
  * test function will be removed lather
  */
  void test();
  void printHeaders();
};

#endif
