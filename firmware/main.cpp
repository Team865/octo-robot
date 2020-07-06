
#include <memory>
#include "command_datasend.h"
#include "command_scheduler.h"
#include "command_process_input.h"
#include "debug_esp8266.h"
#include "hardware_esp8266.h"
#include "net_esp8266.h"
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
  auto motorA = std::make_shared<Command::Motor>(
                        hardware, debug, wifi, 
                        HWI::Pin::MOTOR0_PIN0, HWI::Pin::MOTOR0_PIN1 );
  auto motorB = std::make_shared<Command::Motor>(
                        hardware, debug, wifi, 
                        HWI::Pin::MOTOR1_PIN0, HWI::Pin::MOTOR1_PIN1 );
  auto encoderA = std::make_shared<Command::Encoder>(
                        hardware, debug, wifi, 
                        HWI::Pin::ENCODER0_PIN0, HWI::Pin::ENCODER0_PIN1 );
  auto encoderB = std::make_shared<Command::Encoder>(
                        hardware, debug, wifi, 
                        HWI::Pin::ENCODER1_PIN0, HWI::Pin::ENCODER1_PIN1 );
  auto dataSend = std::make_shared<Command::DataSend>( debug, wifi, 
                        encoderA, encoderB );

  auto commandProcessor= std::make_shared<Command::ProcessCommand>( 
                        wifi, hardware, debug, 
                        time,   
                        motorA, motorB,
                        encoderA, encoderB,
                        hst,
                        scheduler,
                        dataSend );

  scheduler->addCommand( commandProcessor);
  scheduler->addCommand( motorA );
  scheduler->addCommand( motorB );
  scheduler->addCommand( encoderA );
  scheduler->addCommand( encoderB );
  scheduler->addCommand( wifi );
  auto connection = wifi->getShared();
  scheduler->addCommand( connection );
  scheduler->addCommand( hst );
  scheduler->addCommand( dataSend );
}
