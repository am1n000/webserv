/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:58:07 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/24 16:56:33 by otossa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Request.hpp"
#include "Server.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <set>

class Cgi {
  public:
    typedef std::map<std::string, std::string>::iterator header_iterator;

  private:
    Request *request_;
    std::vector<char *> env_;
    std::vector<char*> args_;  
    std::string  response_file_name_;
    int request_fd_;
    int response_fd_;
    int process_id_;
    int exit_status_;
    int finished_;
    char *makeVar_(std::string key, const std::string& value);
    void prepareKey_(std::string& key);
    void prepareHttpHeaders_();
  public:
    Cgi();
    Cgi(Request* request);
    ~Cgi();
    
    /*
     * data
     */
    bool in_progress;
    static const char* common_headers_array[];
    static std::set<std::string>  common_headers;

    /*
     * handlers
     */
    void appendStream(const char *buffer, size_t buffer_size);
    void executeCgi();
    void prepareEnv();
    void prepareArgs();

    /*
     * file handlers
     */
    void openFiles();
    void closeFiles();

    /*
    * getter
    */
    int         getResponseFileDescriptor();
    std::string getResponseFileName();
    bool        isFinished();
    /*
     * setters
     */
      static void setCommonHeaders();

  
};
#endif
