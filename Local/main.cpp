/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 17:18:52 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/09 19:33:11 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Webserv.hpp"
#include "../Includes/Cgi.hpp"
#include <iostream>

typedef std::map<std::string, std::string> strmap;

int main ()
{
  Cgi c;
  c.testCgi("walo");
}
