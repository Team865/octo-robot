
#include <memory>
#include "net_esp8266.h"
#include "hardware_esp8266.h"
#include "debug_esp8266.h"
#include "action_manager.h"
#include "action_process_command.h"
#include "time_esp8266.h"
#include "time_manager.h"
#include "wifi_secrets.h"

std::shared_ptr<Action::Manager> action_manager;

void loop() {
  Time::TimeUS pause = action_manager->periodic();
  if ( pause != Time::TimeUS(0) )
  {
    Time::TimeMS ms = Time::UStoMS( pause );
    Time::TimeUS usRemainder = Time::TimeUS( pause - Time::MStoUS(ms ));
    delay( ms.get() );
    delayMicroseconds( usRemainder.get() );
  }
}

void setup() {
  auto debug     = std::make_shared<DebugESP8266>();
  auto wifi      = std::make_shared<WifiInterfaceEthernet>(debug);
  auto hardware  = std::make_shared<HardwareESP8266>();
  auto timeNNTP  = std::make_shared<TimeESP8266>( debug );
  auto time      = std::make_shared<Time::Manager>( timeNNTP );
  auto motor = std::make_shared<Action::Motor>(
      hardware, debug, wifi, HWI::Pin::MOTOR0_PIN0, HWI::Pin::MOTOR0_PIN1 );
  auto commandProcessor= std::make_shared<Action::ProcessCommand>( wifi, hardware, debug, time, motor );

  action_manager = std::make_shared<Action::Manager>( wifi, hardware, debug );
  action_manager->addAction( commandProcessor);
  action_manager->addAction( time );
  action_manager->addAction( motor );
  action_manager->addAction( wifi );
}
