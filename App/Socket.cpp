#include "../Includes/socket.hpp"

sock::sock()
{}

sock::~sock()
{}

sock::sock(sock const &obj)
{}


sock& sock::operator=(sock const &obj)
{}


int create_socket()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        std::cerr << "error: socket" << std::endl;
        return (1);
    }
    std::cout << "socket created successfully!" << std::endl;
    return (sock_fd);
}