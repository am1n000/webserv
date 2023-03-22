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
    for (int i = 0; i < 3; i++)
    {
        std::cout << "i" << std::endl;
        for (int j = 0; j < 5; j++)
        {
            if (j == 2)
                break;
            std::cout << " j" << std::endl;
        }
    }
}