/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:58:07 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/05 15:57:03 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Request.hpp"
#include "Server.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>

class Cgi {
  private:
    Request *request_;
    std::vector<char *> env_;
    std::vector<char*> args_;  
    std::string  response_file_name_;
    int request_fd_;
    int response_fd_;
  public:
    Cgi();
    Cgi(Request* request);
    ~Cgi();
    
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

  int getResponseFileDescriptor();
  std::string getResponseFileName();
};
#endif
