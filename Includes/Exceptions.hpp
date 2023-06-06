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
	protected:
		std::string _code;
	public:
		statusCodeExceptions(std::string code);
		~statusCodeExceptions() throw();
		virtual const char* what() const throw() = 0;
		std::string getValue() const throw();
};

class MovedPermanentlyException : public statusCodeExceptions
{
	private :
		std::string _redirection;
	public :
		MovedPermanentlyException(std::string redirection);
		~MovedPermanentlyException() throw();
		virtual const char* what() const throw();
};

class BadRequestException : public statusCodeExceptions
{
	public :

		BadRequestException();
		~BadRequestException() throw();
		virtual const char* what() const throw();
};

class ForbiddenException : public statusCodeExceptions
{
	public :

		ForbiddenException();
		~ForbiddenException() throw();
		virtual const char* what() const throw();
};

class FileNotFoundException : public statusCodeExceptions
{
	public :
		FileNotFoundException();
		~FileNotFoundException() throw();
		virtual const char* what() const throw();
};

class MethodNotAllowedException : public statusCodeExceptions
{
	public :

		MethodNotAllowedException();
		~MethodNotAllowedException() throw();
		virtual const char* what() const throw();
};

class RequestTimeoutException : public statusCodeExceptions
{
	public :

		RequestTimeoutException();
		~RequestTimeoutException() throw();
		virtual const char* what() const throw();
};

class ConflictException : public statusCodeExceptions
{
	public :

		ConflictException();
		~ConflictException() throw();
		virtual const char* what() const throw();
};

class RequestURITooLongException : public statusCodeExceptions
{
	public :

		RequestURITooLongException();
		~RequestURITooLongException() throw();
		virtual const char* what() const throw();
};

class InternalServerErrorException : public statusCodeExceptions
{
	public :

		InternalServerErrorException();
		~InternalServerErrorException() throw();
		virtual const char* what() const throw();
};

class NotImplementedException : public statusCodeExceptions
{
	public :

		NotImplementedException();
		~NotImplementedException() throw();
		virtual const char* what() const throw();
};

class PayloadTooLargeException : public statusCodeExceptions
{
	public :

		PayloadTooLargeException();
		~PayloadTooLargeException() throw();
		virtual const char* what() const throw();
};

class BadConfigException
{
	private:
		std::string message_;
	public:
		BadConfigException() throw() ;
		BadConfigException(const std::string message) throw() ;
		~BadConfigException() throw();
		virtual const char* what() const throw();
};


#endif
