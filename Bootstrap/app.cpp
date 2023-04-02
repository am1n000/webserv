#include "../Includes/Config.hpp"
#include "../Includes/Settings.hpp"

void app() {

  // loading configuration
  Settings::boot();
  Config::boot();



  Config::cleanup();
}
