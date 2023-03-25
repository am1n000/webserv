/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 00:07:43 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/25 02:38:33 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include "Includes/Config.hpp"
#include "Includes/Server.hpp"
#include "Includes/Tockenizer.hpp"
#include "Includes/helpers.hpp"
#include "dev/dev.hpp"



int main ()
{

  Location::setMembers();
  try {
  Config c;
  c.parse();
  } catch (std::runtime_error& error) {

    std::cerr << "Error: " << error.what() << std::endl;
  }

}
