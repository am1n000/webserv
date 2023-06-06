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


statusCodeExceptions::statusCodeExceptions(std::string code) : _code(code) {}
statusCodeExceptions::~statusCodeExceptions() throw() {}

std::string statusCodeExceptions::getValue() const throw()
{
	return (_code);
}

MovedPermanentlyException::MovedPermanentlyException(std::string redirect) : statusCodeExceptions("301"), _redirection(redirect) {}
MovedPermanentlyException::~MovedPermanentlyException() throw() {};
const char* MovedPermanentlyException::what() const throw()
{
	return (this->_redirection.c_str());
}

BadRequestException::BadRequestException() : statusCodeExceptions("400") {}
BadRequestException::~BadRequestException() throw() {};
const char* BadRequestException::what() const throw()
{
	return ("Bad Request");
}

ForbiddenException::ForbiddenException() : statusCodeExceptions("403") {}
ForbiddenException::~ForbiddenException() throw() {};
const char* ForbiddenException::what() const throw()
{
	return ("Forbidden");
}

FileNotFoundException::FileNotFoundException() : statusCodeExceptions("404") {}
FileNotFoundException::~FileNotFoundException() throw() {};
const char* FileNotFoundException::what() const throw()
{
	return ("Not Found");
}

MethodNotAllowedException::MethodNotAllowedException() : statusCodeExceptions("405") {}
MethodNotAllowedException::~MethodNotAllowedException() throw() {};
const char* MethodNotAllowedException::what() const throw()
{
	return ("Method Not Allowed");
}

RequestTimeoutException::RequestTimeoutException() : statusCodeExceptions("408") {}
RequestTimeoutException::~RequestTimeoutException() throw() {};
const char* RequestTimeoutException::what() const throw()
{
	return ("Request Timeout");
}

ConflictException::ConflictException() : statusCodeExceptions("409") {}
ConflictException::~ConflictException() throw() {};
const char* ConflictException::what() const throw()
{
	return ("Conflict");
}

RequestURITooLongException::RequestURITooLongException() : statusCodeExceptions("414") {}
RequestURITooLongException::~RequestURITooLongException() throw() {};
const char* RequestURITooLongException::what() const throw()
{
	return ("Request-URI Too Long");
}

InternalServerErrorException::InternalServerErrorException() : statusCodeExceptions("500") {}
InternalServerErrorException::~InternalServerErrorException() throw() {};
const char* InternalServerErrorException::what() const throw()
{
	return ("Internal Server Error");
}

NotImplementedException::NotImplementedException() : statusCodeExceptions("501") {}
NotImplementedException::~NotImplementedException() throw() {};
const char* NotImplementedException::what() const throw()
{
	return ("Not Implemented");
}

PayloadTooLargeException::PayloadTooLargeException() : statusCodeExceptions("413") {}
PayloadTooLargeException::~PayloadTooLargeException() throw() {};
const char* PayloadTooLargeException::what() const throw()
{
	return ("payload too large");
}
