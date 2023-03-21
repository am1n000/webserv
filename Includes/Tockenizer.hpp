/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tockenizer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 22:14:56 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/21 20:56:23 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TOCKENIZER_HPP
#define TOCKENIZER_HPP
#include <string>
#include <iostream>

class Tockenizer {
public:
  typedef size_t size_type;
  typedef std::string data_type;
  typedef data_type &reference;
  typedef data_type *pointer;
  typedef const std::string& const_reference;
  private:
    data_type data_;
    size_type current_;
     
public:
  Tockenizer();
  Tockenizer(const_reference);
  data_type  getLine();
  data_type getNoneEmptyLine();
  data_type getNextToken(char c = ' ');
  data_type getNextScope();
  data_type data();
  bool  end();
  ~Tockenizer();
};

#endif // !TOCKENIZER_HPP
