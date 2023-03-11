/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 17:49:09 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/11 18:38:57 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <fstream>
#include <string>

class Config {
  private:
    std::string path_;
    std::string buffer_;
    std::ifstream*  file_;
    std::streamsize file_size_;
  public:
    Config();
    Config(const std::string& path);

    void setFileSize();
    std::string read();
    ~Config();
};
