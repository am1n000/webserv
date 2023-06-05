/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 11:02:13 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/26 10:17:20 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Exceptions.hpp"


std::string statusCodeExceptions::getValue() const throw()
{
	return _code;
}

const char* FileNotFoundException::what() const throw()
{
	return ("Not Found");
}

const char* InternalServerErrorException::what() const throw()
{
	return ("Internal Server Error");
}

const char* BadRequestException::what() const throw()
{
	return ("Bad Request");
}

const char* ForbiddenException::what() const throw()
{
	return ("Forbidden");
}

const char* MovedPermanentlyException::what() const throw()
{
	return ("Moved Permanently");
}

const char* MethodNotAllowedException::what() const throw()
{
	return ("Method Not Allowed");
}

const char* ConflictException::what() const throw()
{
	return ("Conflict");
}

const char* RequestURITooLongException::what() const throw()
{
	return ("Request-URI Too Long");
}

const char* NotImplementedException::what() const throw()
{
	return ("Not Implemented");
}

const char* PayloadTooLargeException::what() const throw()
{
	return ("payload too large");
}
