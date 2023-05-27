/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 17:59:26 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/26 10:16:13 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <iostream>


class statusCodeExceptions : public std::exception
{
	public:
		statusCodeExceptions(std::string code) : _code(code) {}
		~statusCodeExceptions() throw() {}
		virtual const char* what() const throw() = 0;
		std::string getValue() const throw();
	protected:
		std::string _code;
};

class MovedPermanentlyException : public statusCodeExceptions
{
	public :

		MovedPermanentlyException() : statusCodeExceptions("301") {}
		~MovedPermanentlyException() throw() {};
		virtual const char* what() const throw();
};

class BadRequestException : public statusCodeExceptions
{
	public :

		BadRequestException() : statusCodeExceptions("400") {}
		~BadRequestException() throw() {};
		virtual const char* what() const throw();
};

class ForbiddenException : public statusCodeExceptions
{
	public :

		ForbiddenException() : statusCodeExceptions("403") {}
		~ForbiddenException() throw() {};
		virtual const char* what() const throw();
};

class FileNotFoundException : public statusCodeExceptions
{
	public :
		FileNotFoundException() : statusCodeExceptions("404") {}
		~FileNotFoundException() throw() {};
		virtual const char* what() const throw();
};

class MethodNotAllowedException : public statusCodeExceptions
{
	public :

		MethodNotAllowedException() : statusCodeExceptions("405") {}
		~MethodNotAllowedException() throw() {};
		virtual const char* what() const throw();
};

class ConflictException : public statusCodeExceptions
{
	public :

		ConflictException() : statusCodeExceptions("409") {}
		~ConflictException() throw() {};
		virtual const char* what() const throw();
};

class RequestURITooLongException : public statusCodeExceptions
{
	public :

		RequestURITooLongException() : statusCodeExceptions("414") {}
		~RequestURITooLongException() throw() {};
		virtual const char* what() const throw();
};

class InternalServerErrorException : public statusCodeExceptions
{
	public :

		InternalServerErrorException() : statusCodeExceptions("500") {}
		~InternalServerErrorException() throw() {};
		virtual const char* what() const throw();
};

class NotImplementedException : public statusCodeExceptions
{
	public :

		NotImplementedException() : statusCodeExceptions("501") {}
		~NotImplementedException() throw() {};
		virtual const char* what() const throw();
};

class PayloadTooLargeException : public statusCodeExceptions
{
	public :

		PayloadTooLargeException() : statusCodeExceptions("413") {}
		~PayloadTooLargeException() throw() {};
		virtual const char* what() const throw();
};

class BadConfigException {
private:
  std::string message_;
public:
  BadConfigException() throw() ;
  BadConfigException(const std::string message) throw() ;
  ~BadConfigException() throw();
  virtual const char* what() const throw();
};


#endif
