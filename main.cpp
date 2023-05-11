#include "Includes/App.hpp"
#include "Includes/Config.hpp"
#include "Includes/multiplexers/Kqueue.hpp"
#include "Includes/multiplexers/Select.hpp"
#include "Includes/multiplexers/Poll.hpp"

int main ()
{
	app();
  	std::vector<Server *> servers = Config::get()->getServers();
	Kqueue *K = Kqueue::getInstance();
	K->serve();
	Select *S = Select::getInstance();
	S->serve();
	// Poll *P = Poll::getInstance();
	// P->serve();
}     
