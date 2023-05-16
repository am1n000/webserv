/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tockenizer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 23:22:52 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/16 14:22:45 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Tockenizer.hpp"
#include <cctype>
#include <stdexcept>
#include <vector>

Tockenizer::Tockenizer() : current_(0) {}

Tockenizer::Tockenizer(Tockenizer::const_reference data) : data_(data), current_(0) {}

std::string Tockenizer::getLine() {
  size_type size(0);
  size_type begin;

  while (current_ + size < this->data_.length() && this->data_[current_ + size] != '\n')
    size++;
  begin = this->current_;
  this->current_ = current_ + size;
  if(size == 0)
    return "";
  return this->data_.substr(begin, size);
}

Tockenizer::data_type Tockenizer::getNoneEmptyLine() {
  data_type line;
  while(current_ < this->data_.length())
    {
      line = this->getLine();
      if (line[0] != '\0')
        break;
      current_++;
    }
  return line;
}

std::string Tockenizer::getNextToken(char c) {
  size_type size(0);
  size_type begin;
  data_type result;

  begin = this->current_;
  while(std::isspace(this->data_[begin]))
    begin++;
  while (begin + size < this->data_.length() &&
         (data_[begin + size] != c || data_[begin + size] == '\n')) {
    size++;
  }
  this->current_ = begin + size + 1 ;
  if(size == 0)
    return result;
  result = this->data_.substr(begin, size);
  if(result[result.length() - 1] == '\n')
    result[result.length() - 1] = '\0';
  return result;
}

std::string Tockenizer::getNextScope() {
  size_type begin;
  size_type size(0);
  size_type bracket;

  begin = this->current_; 
  bracket = 0;
  while(begin < data_.length() &&this->data_[begin] != '{')
    begin++;
  do {
    if (data_[begin + size] == '{')
      bracket++;
    if (data_[begin + size] == '}')
      bracket--;
    size++;
  } while (begin + size < this->data_.length() && bracket > 0);
  this->current_ = begin + size;
  if (begin + size >= data_.length())
    throw std::runtime_error("unclosed brackets");
  return this->data_.substr(begin + 1, size - 2);
}


Tockenizer::size_type Tockenizer::getCurrent() const { return this->current_; }

Tockenizer::data_type Tockenizer::data() { return this->data_; }

bool Tockenizer::end() { return this->current_ >= this->data_.length() || this->end_; }

Tockenizer::~Tockenizer() {}

