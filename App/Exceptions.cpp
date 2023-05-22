/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 10:58:06 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/22 11:05:22 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Exceptions.hpp"
#include <cstdlib>

BadConfigException::BadConfigException() throw() {
  std::cerr << "Bad configuration error: " << this->message_ << std::endl;
  exit(1);
}

BadConfigException::BadConfigException(const std::string message) throw(): message_(message) {
  std::cerr << "Bad configuration error: " << this->message_ << std::endl;
  exit(1);
}

BadConfigException::~BadConfigException() throw() {}

const char* BadConfigException::what() const throw() {
  return this->message_.data();
}
