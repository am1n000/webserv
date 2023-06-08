#include "../Includes/App.hpp"
#include "../Includes/Exceptions.hpp"

void app(const std::string config_file, int flag)
{

	// loading configuration
	Server::setDictionary();
  Cgi::setCommonHeaders();
	Settings::boot();
  std::cout << "booting the server" << std::endl;
	Config::boot(config_file);

  if(Config::get()->getServers().empty())
    throw BadConfigException("there is no servers in config file");
  // if(Config::get()->getMimeTypes().empty())
  //   throw BadConfigException("there is no mimetypes");
  std::cout << "server booted" << std::endl;
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
	else if (flag == 1)
	{
		Select *S = Select::getInstance();
		S->serve();
	}
	else
	{
		Poll *P = Poll::getInstance();
		P->serve();
	}

	Config::cleanup();
  Settings::cleanup();
}
