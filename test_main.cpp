#include <iostream>
#include <fstream>


class test
{
    public:
        std::ifstream file;
        test() {};
        ~test() {};
};

int main()
{
    char *buff = new char[1000];
    buff[0] = 'h';
    buff[1] = 'e';
    buff[2] = 'l';
    buff[3] = 'l';
    buff[4] = 'o';
    buff[5] = 'w';
    buff[6] = 'o';
    buff[7] = 'r';
    buff[8] = 'l';
    buff[9] = 'd';
    buff[500] = 'a';
    std::string test ="hi  and ";
    test += buff;
    std::cout << test;    
}