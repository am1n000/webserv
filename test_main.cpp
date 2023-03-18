
#include <iostream>
#include <vector>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <sys/event.h>
// #include <sys/time.h>
// #include <stdlib.h>
// #include <fcntl.h>


void test()
{
        std::vector<int*> tests;
        for (int i = 0; i < 5; i++)
        {
            int *a = new int;
            *a = i;
            tests.push_back(a);
        }
        delete tests[2];
        std::cout << *(tests[2]) << std::endl;

}
int main()
{
    test();
    system( "leaks a.out");
}