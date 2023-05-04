#include "../Includes/helpers.hpp"
#include <iostream>
int main ()
{
  
  std::cout << helpers::timeBasedName(".request") << std::endl;
  std::cout << helpers::timeBasedName(".response") << std::endl;
  std::cout << helpers::timeBasedName(".file") << std::endl;
  return 0;
}
