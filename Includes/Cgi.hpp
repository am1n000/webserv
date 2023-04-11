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

#include <fstream>
#include <iostream>
#include <sys/_types/_size_t.h>

class Cgi {
  private:
    std::fstream file_;
    std::string cgi_path_;
  public:
    Cgi();
    Cgi(const std::string& path);
    ~Cgi();
    
    void openFile(const std::string& path);
    void appendStream(const char *buffer, size_t buffer_size);
    void testCgi(std::string file, std::string  cl);
};
#endif
