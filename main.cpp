#include "Includes/App.hpp"

int main (int argc, char **argv)
{
	if (argc != 2 && argc != 3)
	{
		std::cerr << "usage : webserv [configuration file]" <<std::endl;
		std::cerr << "        webserv [configuration file] [--with-select_module]" <<std::endl;
		std::cerr << "        webserv [configuration file] [--with-poll_module]" <<std::endl;
		return (1);
	}
	std::ifstream config_file(argv[1]);
	if (!config_file.is_open())
	{
		std::cerr << "error: can't read configuration file" << std::endl;
		return (1);
	}
	config_file.close();
	int flag = 0;
	if (argc == 3)
	{
		std::string fla_g = argv[2];
		if (fla_g == "--with-select_module")
			flag = 1;
		else if (fla_g == "--with-poll_module")
			flag = 2;
		else
		{
			std::cerr << "usage : webserv [configuration file]" <<std::endl;
			std::cerr << "        webserv [configuration file] [--with-select_module]" <<std::endl;
			std::cerr << "        webserv [configuration file] [--with-poll_module]" <<std::endl;
			return (1);
		}
	}
	app(argv[1], flag);
}     
