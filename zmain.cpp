#include <iostream>
#include <vector>

#include <sys/event.h>
#include <sys/time.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <vector>
// #include <unistd.h>
// #include <fcntl.h>
// #include <string>
// #include <fstream>
// #include <sys/socket.h>
// #include <iostream>
// #include <map>


class test
{
		
	public :
		int _kq;
		test() {};
		~test() {};
		void 	print() {std::cout << _kq << std::endl;};
		void	event_loop() { _kq += 5;};
};

void d(test *ab)
{
	delete ab;
}

int main()
{
	int a = select();
}