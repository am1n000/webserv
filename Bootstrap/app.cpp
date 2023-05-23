#include "../Includes/Config.hpp"
#include "../Includes/Settings.hpp"

void app() {

  // loading configuration
  Server::setDictionary();
  Settings::boot();
  Config::boot();



  // Config::cleanup();
}
