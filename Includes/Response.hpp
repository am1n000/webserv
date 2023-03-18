#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>

class response
{
    public:
        response();
        ~response();
        std::string filename;
        std::string media;
        int 		fd;
};


#endif