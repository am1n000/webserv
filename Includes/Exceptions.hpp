/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 17:59:26 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/29 18:00:37 by hchakoub         ###   ########.fr       */
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
		std::string getValue() const throw() {
			return _code;
		}
	protected:
		std::string _code;
};

class FileNotFound : public statusCodeExceptions
{
	public :
		FileNotFound() : statusCodeExceptions("404") {}
		~FileNotFound() throw() {};
		virtual const char* what() const throw()
		{
			return ("Not Found");
		}

};

class InternalServerError : public statusCodeExceptions
{
	public :

		InternalServerError() : statusCodeExceptions("500") {}
		~InternalServerError() throw() {};
		virtual const char* what() const throw()
		{
			return ("Internal Server Error");
		}

};

class BadRequestException : public statusCodeExceptions
{
	public :

		BadRequestException() : statusCodeExceptions("400") {}
		~BadRequestException() throw() {};
		virtual const char* what() const throw()
		{
			return ("Internal Server Error");
		}

};


#endif