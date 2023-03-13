#include <iostream>
#include <string>
#include <utility>
#include "Includes/Config.hpp"
#include "Includes/Server.hpp"
#include "Includes/helpers.hpp"
#include "dev/dev.hpp"


void test() {
  std::string str = "Lorem ipsum dolor sit amet, qui minim labore adipisicing minim sint cillum sint consectetur cupidatat.";
  char *token = std::strtok(&str[0]," ");
  token = std::strtok(NULL, " ");
  std::cout << "this is token: " << token << std::endl;
  std::cout << str << std::endl;
}

int main ()
{
  Config c;

  c.read();

  std::string  buffer = c.getConfigBuffer();
  char *token = std::strtok(&buffer[0], " ");
  while(token && std::string(token) != "location")
    token = std::strtok(NULL, " ");

  // token = std::strtok(NULL, " ");
  // helpers::recoverToken(token, ' ');
  // std::string scope = helpers::getBracketsContent(token);
  // helpers::recoverToken(token, ' ');
  // 
  std::cout << token << std::endl;
  // dev::br();
  // std::cout<< "here : " << token + scope.length() << std::endl;
  

}
