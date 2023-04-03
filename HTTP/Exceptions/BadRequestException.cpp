/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BadRequestException.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 14:56:55 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/29 22:40:18 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Exceptions/BadRequestException.hpp"

const char *BadRequestException::what() const throw() {
  return "Bad request";
}
