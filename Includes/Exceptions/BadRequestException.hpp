/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BadRequestException.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 14:50:50 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/29 14:59:33 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BADREQUESTEXCEPTION_HPP
#define BADREQUESTEXCEPTION_HPP

#include <exception>

class BadRequestException : public std::exception {

public:
  const char * what() const throw();
};
#endif // !DEBUG
