#include "../Includes/helpers.hpp"
#include <cctype>
#include <exception>
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
