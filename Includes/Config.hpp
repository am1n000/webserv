/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 17:49:09 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/13 11:06:45 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <cstddef>
#include <fstream>
#include <string>

class Config {
  public:
    typedef std::size_t size_type;
  private:
    std::string path_;
    std::string buffer_;
    std::ifstream*  file_;
    std::streamsize file_size_;
  public:
    Config();
    Config(const std::string& path);

    void setFileSize();
    void  closeFile();
    void parseServer();
    void parse();
    std::string getConfigBuffer() const;
    std::string read();
    std::string getServerString(size_type start);
    ~Config();
};
