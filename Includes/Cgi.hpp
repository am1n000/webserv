/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 23:58:07 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/11 00:40:46 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Request.hpp"
#include <fstream>
#include <iostream>
#include <sys/_types/_size_t.h>
#include <vector>

class Cgi {
  private:
    Request request_;
    std::fstream file_;
    std::string cgi_path_;
    std::vector<char *> env_;
    std::vector<char*> args_;  
  public:
    Cgi();
    Cgi(Request& request);
    ~Cgi();
    
    void openFile(const std::string& path);
    void appendStream(const char *buffer, size_t buffer_size);
    void testCgi();

    void prepareEnv();
    void prepareArgs();
};
#endif
