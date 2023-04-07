#include "Includes/Client.hpp"
#include "Includes/Client.hpp"
#include "Includes/ClientPoll.hpp"
#include "Includes/App.hpp"
#include "Includes/Config.hpp"
#include "Includes/Server.hpp"
#include "Includes/multiplexers/Kqueue.hpp"
#include "Includes/multiplexers/Select.hpp"
#include "Includes/multiplexers/Poll.hpp"

//!change map to vector for both select and poll clients
int main ()
{
	app();
  	std::vector<Server *> servers = Config::get()->getServers();
	// Kqueue::serve(servers);
	Select::serve(servers);
	// Poll::serve(servers);
}     