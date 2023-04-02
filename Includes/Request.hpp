/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:49:20 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/30 15:48:08 by hchakoub         ###   ########.fr       */
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

// must be included on the response, i defined it here to avoid conflit
// also fstream header must be included on response header
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
  bool header_completed_;
  bool body_completed_;
  method_type request_method_;
  std::string request_uri_;
  std::string http_version_;
  std::map<std::string, std::string> request_headers_;
  size_type content_length;
  //the file is temporary just to work with amine's code
  // to be changed lather
  s_file file_;

  /*
  * private methods
  */
  void parseMetadata_(const std::string& metadata);
  void pushHeaders_();
  

public:
  Request();
  Request(size_type buffer_size);
  Request(char *buffer, size_type read_size, size_type buffer_size = BUFFER_SIZE);

  size_type buffer_size;
  int appendBuffer(char *buffer, size_type recieved_size);
  bool isHeaderCompleted();
  bool isBodyCompleted();
  void parseHeader();

  /*
  * setters
  */
  void setMethod(const std::string& method);
  void setRequestUri(const std::string& uri);
  void setHttpVersion(const std::string& version);
  void setHeaderString();
  void setContentLength();

  /*
   * getters
   */

  size_type getContentLength() ;
  int getRequestMethod() const;
  // temoraty to be changed with the file struct
  s_file getFile();

  /*
  * test function will be removed lather
  */
  void test();
};

#endif
