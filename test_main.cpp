#include <iostream>
#include <fstream>
#include <vector>


class test
{
    public:
        std::ifstream file;
        test() {};
        ~test() {};
        int a;
};

int main()
{
    test *a1 = new test;
    a1->a = 1;
    test *a2 = new test;
    a1->a = 2;
    test *a3 = new test;
    a1->a = 3;
    std::vector<test*> s;
    s.push_back(a1);
    s.push_back(a2);
    s.push_back(a3);
    std::cout << s[0].
}