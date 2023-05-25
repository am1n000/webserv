#include "../Includes/App.hpp"

void app(char *config_file, int flag)
{

	// loading configuration
	Server::setDictionary();
	Settings::boot();
	Config::boot(config_file);
	(void)config_file;
	if (flag == 0)
	{
		#ifdef __APPLE__
			Kqueue *K = Kqueue::getInstance();
			K->serve();
		#elif __linux__
			Epoll *E = Epoll::getInstance();
			E->serve();
		#endif
	}
	if (flag == 1)
	{
		Select *S = Select::getInstance();
		S->serve();
	}
	else
	{
		Poll *P = Poll::getInstance();
		P->serve();
	}

	// Config::cleanup();
}
