#ifndef APP_HPP
#define APP_HPP


#include "Config.hpp"
#include "Settings.hpp"
#include "multiplexers/Kqueue.hpp"
#include "multiplexers/Epoll.hpp"
#include "multiplexers/Select.hpp"
#include "multiplexers/Poll.hpp"

void app(char *config_file, int flag);
#endif // !APP_hpp
