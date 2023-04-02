/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 17:18:52 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/02 17:38:58 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Webserv.hpp"
#include <iostream>

typedef std::map<std::string, std::string> strmap;
int main ()
{
  app();

  std::map<std::string, std::string> mp = Config::get()->getMimeTypes();
  // for(strmap::iterator it = mp.begin(); it != mp.end(); it++)
  //   std::cout << it->first << " " << it->second << std::endl;
    
  strmap::iterator it = mp.find("sdfsmp4");
  if(it != mp.end())
    std::cout << it->second << std::endl;
  else
    std::cout << "makaynach" << std::endl;

  return 0;
}
