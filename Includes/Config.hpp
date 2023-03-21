/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 17:49:09 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/21 01:53:39 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>
#include "Server.hpp"
#include "Tockenizer.hpp"

//this header for dev only
#include "../dev/dev.hpp"
#include "helpers.hpp"

class Config {
  public:
    typedef std::size_t size_type;
  private:
    std::string path_;
    std::string buffer_;
    std::ifstream*  file_;
    std::streamsize file_size_;
    Tockenizer *tockenizer_;
    std::vector<Server*> servers_;
  public:
    Config();
    Config(const std::string& path);

    std::string read();
    void setFileSize();
    void  closeFile();
    void parse();
    std::string getConfigBuffer() const;
    void pushServer(const std::string& serverString);
    ~Config();
};
