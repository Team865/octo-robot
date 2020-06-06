
#include <memory>
#include "command_wrangler.h"
#include "net_esp8266.h"
#include "hardware_esp8266.h"
#include "debug_esp8266.h"
#include "action_manager.h"
#include "time_esp8266.h"
#include "time_manager.h"
#include "data_mover.h"
#include "wifi_secrets.h"

std::shared_ptr<ActionManager> action_manager;

void loop() {
  unsigned int pause = action_manager->loop();
  if ( pause != 0 )
  {
    int ms = pause / 1000;
    int usRemainder = pause % 1000;
    delay( ms );
    delayMicroseconds( usRemainder );
  }
}

void setup() {
  auto debug     = std::make_shared<DebugESP8266>();
  auto wifi      = std::make_shared<WifiInterfaceEthernet>(debug);
  auto hardware  = std::make_shared<HardwareESP8266>();
  auto timeNNTP  = std::make_shared<TimeESP8266>( debug );
  auto time      = std::make_shared<TimeManager>( timeNNTP );
  auto motor = std::make_shared<Action::Motor>(
      hardware, debug, wifi, HWI::Pin::MOTOR0_PIN0, HWI::Pin::MOTOR0_PIN1 );
  auto commandWrangler = std::make_shared<FS::CommandWrangler>( wifi, hardware, debug, time, motor );

  action_manager = std::make_shared<ActionManager>( wifi, hardware, debug );
  action_manager->addAction( commandWrangler );
  action_manager->addAction( time );
  action_manager->addAction( motor );
  action_manager->addAction( wifi );
}
