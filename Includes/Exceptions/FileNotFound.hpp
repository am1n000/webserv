#ifndef FILENOTFOUND_HPP
#define FILENOTFOUND_HPP

#include <exception>
#include <iostream>


class FileNotFound : public std::ios_base::failure
{
	public:
		FileNotFound() : std::ios_base::failure("File not found") {}
};

#endif