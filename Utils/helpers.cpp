/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 11:45:31 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/25 02:13:58 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/helpers.hpp"
#include <cctype>
#include <exception>
#include <stdexcept>
#include <string>

std::string helpers::trim(const std::string str, char c) {
  std::string result;
  int start;
  int end;

  start = 0;
  end = str.length();

  while(str[start] == c)
    start++;

  while (str[end - 1] == c || str[end - 1] == '\r')
    end--;

  result = str.substr(start, str.length() - (str.length() - end) - start);
  return result;
}

void helpers::recoverToken(char *token, char replacment) {
  int i = 0;

  while(token[i])
    i++;
  token[i] = replacment;
}

std::string helpers::getBracketsContent(char *buffer)  {
  int         openBrackets;
  int         size;
  int         start;
  std::string serverString;

  openBrackets = 1;
  size = 0;
  start = 0;
  while (std::isspace(buffer[start]))
    start++;
  if(buffer[start] != '{')
    throw std::exception();
  start++;
  while(openBrackets > 0) {
    if(buffer[start + size] == '{')
      openBrackets++;
    else if (buffer[start + size] == '}')
      openBrackets--;
    size++;
  }
  return std::string(buffer+start, buffer+start+size-1);
}

std::string helpers::getNextScop(char *buffer)  {
  int         openBrackets;
  int         size;
  int         start;
  std::string serverString;

  openBrackets = 1;
  size = 0;
  start = 0;
  while (buffer[start] != '{')
    start++;
  if(buffer[start] != '{')
    throw std::exception();
  start++;
  while(buffer [start + size] && openBrackets > 0) {
    if(buffer[start + size] == '{')
      openBrackets++;
    else if (buffer[start + size] == '}')
      openBrackets--;
    size++;
  }
  if(!buffer [start + size])
    throw std::runtime_error("unclosed scop");
  return std::string(buffer+start, buffer+start+size-1);
}

std::string helpers::unscope(const std::string& scope) {

  size_t start(0);
  size_t size(scope.length() - 1);

  while (std::isspace(scope[start]))
    start++;
  if(scope[start] != '{')
    throw std::runtime_error("invalid scope");
  start++;
  while(std::isspace(scope[size]))
    size--;
  if(scope[size] != '}')
      throw std::runtime_error("invalid scope");
  return scope.substr(start, size - start);
}
