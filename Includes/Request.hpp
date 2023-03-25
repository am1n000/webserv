#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include "helpers.hpp"

class Request {
  public:
    typedef std::map<std::string, std::string>  header_type;

  private:
    std::string                                 _method;
    std::string                                 _uri;
    std::string                                 _http_version;
    header_type                                 _header;
    std::string                                 _request_string;
    std::string                                 _header_string;
    std::string                                 _body_string;
    void                                        _parseRequest();
    void                                        _parseHeader();

  public:
    Request();
    Request(const std::string request);
    Request(const Request &request);

    int append(std::string content) {
      this->_request_string += content;
      return 0;
    }

    std::string getMethod() const;
    void setMethod(const std::string method);

    ~Request();
};
