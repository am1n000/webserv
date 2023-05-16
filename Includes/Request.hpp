/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:49:20 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/16 12:32:54 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <fstream>
#include <sys/socket.h>
#include <iostream>
#include <map>
#include "../Includes/HeaderTockernizer.hpp"
#include "Settings.hpp"
#include "../Includes/Exceptions.hpp"
#include "../Includes/helpers.hpp"

#define BUFFER_SIZE 4096
#define REQUEST_SEPARATOR "\r\n\r\n"
#define TMP_VAR_PATH "/tmp/"
 

// must be included on the response, i defined it here to avoid conflit
// also fstream header must be included on response header

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
  typedef std::map<std::string, std::string> string_map_type;

private:
  HeaderTockernizer *tockenizer_;
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
  std::fstream *body_file_;
  std::string   body_file_name_;
  size_type body_size_;
  std::string extention_;
  std::string filename_;
  std::string requestedRessource_;
  Server *server_;
  std::map<std::string, std::string> request_configuration_;
  size_type buffer_size;
  Location *request_location_;
  // this members for chunked request
  size_type chunk_size_;
  size_type chunk_received_;
  size_type content_length;
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
  bool hasCgi() const;
  bool isAutoIndexed();
  bool isChuncked();
  bool isMethodAllowed();
  void checkHeaderKey(const std::string& key);
  Location* matchLocation();

  /*
   * modifiers
   */

  int appendBuffer(char *buffer, size_type recieved_size);
  void appendBodyFile(const char *buffer, size_type buffer_size);
  void unchunckRequest(std::string& buffer);
  void prepareRequest();

  /*
  * setters
  */

  void setMethod(const std::string& method);
  void setRequestUri(const std::string& uri);
  void setRequestedRessource(const std::string& uri);
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
  const std::string &getRequestedRessource();
  std::string getExtention() const;
  Server* getServer() const;
  std::string getRequestRoot() const;
  std::string getRequestedFileFullPath() const;
  std::string getMimeType() const;
  std::string &getRequestCgi() const;
  std::fstream *getBodyFile();
  std::string& getBodyFileName();
  std::string& getQueryParams();
  Location *getLocation();

  /*
   * hooks
   */

  void headerCompletedEventHook();
  /*
  * test function will be removed lather
  */
  void test();
  void printHeaders();
};

#endif
