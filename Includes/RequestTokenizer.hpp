/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestTokenizer.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 09:39:42 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/22 10:12:18 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_TOKENIZER_HPP
#define REQUEST_TOKENIZER_HPP

#include <map>
#include "./Tockenizer.hpp"
#include "./helpers.hpp"

class RequestTokenizer: public Tockenizer {
private:
  bool isAllowedChar_(const char c);
  public:
    RequestTokenizer();
    RequestTokenizer(const std::string& data);
    ~RequestTokenizer();
    std::string getLine();
    std::map<std::string, std::string> parseHeaders();
    bool isKeyValid(const std::string &key);
};
#endif // !REQUEST_TOKENIZER_HPP
#define REQUEST_TOKENIZER_HPP
