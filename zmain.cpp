#include <dirent.h>
#include <iostream>

class test
{
	private:
		static int _kq;
		test() {};
		
	public :
		~test() {};
		static void print() {std::cout << _kq << std::endl;};
		static void	event_loop() { _kq += 5;};
};
    int test::_kq = 5;
int main()
{
    test::print();
    test::event_loop();
    test::print();

    return 0;
}