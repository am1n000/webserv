/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tockenizer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 22:14:56 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/24 22:40:06 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TOCKENIZER_HPP
#define TOCKENIZER_HPP
#include <string>
#include <iostream>
#include <vector>
#include "../Includes/Exceptions.hpp"

class Tockenizer {
public:
  typedef size_t size_type;
  typedef std::string data_type;
  typedef const std::string& const_reference;
protected:
    data_type data_;
    size_type current_;
     
public:
  Tockenizer();
  Tockenizer(const_reference);
  data_type  getLine();
  data_type getNoneEmptyLine();
  data_type getNextToken(char c = ' ');
  data_type getHeaders();
  data_type getNextScope();
  data_type data();
  bool  end();
  void skipLine();
  void skipSpaces();
  void skipEmptyLines();
  ~Tockenizer();
  static bool inDictionary(const std::string& token, const std::vector<std::string>& dictionary);
};

#endif // !TOCKENIZER_HPP
