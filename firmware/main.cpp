
#include <memory>
#include "net_esp8266.h"
#include "hardware_esp8266.h"
#include "debug_esp8266.h"
#include "command_scheduler.h"
#include "command_process_input.h"
#include "time_esp8266.h"
#include "time_esp8266hst.h"
#include "time_manager.h"
#include "wifi_secrets.h"

std::shared_ptr<Command::Scheduler> scheduler;

void loop() {
  Time::TimeUS pause = scheduler->execute();
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
  auto hst       = std::make_shared<Time::ESP8266_HST>();
  scheduler      = std::make_shared<Command::Scheduler>( 
                        wifi, hardware, debug, hst );
  auto timeNNTP  = std::make_shared<TimeESP8266>( debug );
  auto time      = std::make_shared<Time::Manager>( timeNNTP, hst );
  auto motor = std::make_shared<Command::Motor>(
                        hardware, debug, wifi, 
                        HWI::Pin::MOTOR0_PIN0, HWI::Pin::MOTOR0_PIN1 );
  auto encoder = std::make_shared<Command::Encoder>(
                        hardware, debug, wifi, 
                        HWI::Pin::ENCODER0_PIN0, HWI::Pin::ENCODER0_PIN1 );

  auto commandProcessor= std::make_shared<Command::ProcessCommand>( 
                        wifi, hardware, debug, 
                        time, motor, encoder,
                        hst,
                        scheduler );

  scheduler->addCommand( commandProcessor);
  //scheduler->addCommand( time );
  scheduler->addCommand( motor );
  scheduler->addCommand( encoder );
  scheduler->addCommand( wifi );
  scheduler->addCommand( hst );
}
