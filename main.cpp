#include "Includes/App.hpp"
#include "Includes/Config.hpp"
#include "Includes/multiplexers/Kqueue.hpp"
#include "Includes/multiplexers/Epoll.hpp"
#include "Includes/multiplexers/Select.hpp"
#include "Includes/multiplexers/Poll.hpp"

int main ()
{
	app();
	#ifdef __APPLE__
		Kqueue *K = Kqueue::getInstance();
		K->serve();
	#elif __linux__
		Epoll *E = Epoll::getInstance();
		E->serve();
	#endif
	Select *S = Select::getInstance();
	S->serve();
	Poll *P = Poll::getInstance();
	P->serve();
}     
