
#include <memory>
#include "command_datasend.h"
#include "command_gyro.h"
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
  auto hst       = std::make_shared<Time::ESP8266_HST>();
  auto hardware  = std::make_shared<HW::HardwareESP8266>( hst );
  scheduler      = std::make_shared<Command::Scheduler>( 
                        wifi, hardware, debug, hst );
  auto timeNNTP  = std::make_shared<TimeESP8266>( debug );
  auto time      = std::make_shared<Time::Manager>( timeNNTP, hst );
  auto motorA = std::make_shared<Command::Motor>( hardware, debug, 0);
  auto motorB = std::make_shared<Command::Motor>( hardware, debug, 1);
  auto encoderA = std::make_shared<Command::Encoder>( hardware, debug, wifi, hst, 0 );
  auto encoderB = std::make_shared<Command::Encoder>( hardware, debug, wifi, hst, 1 );
  auto sr04     = std::make_shared<Command::SR04> ( 
                        hardware, debug, wifi, hst,
                        HW::Pin::SR04_TRIG, HW::Pin::SR04_ECHO );
  auto gyro     = std::make_shared<Command::Gyro> ( hardware, debug );
          
  auto dataSend = std::make_shared<Command::DataSend>( debug, wifi, 
                        encoderA, encoderB, sr04, gyro, hardware );

  auto commandProcessor= std::make_shared<Command::ProcessCommand>( 
                        wifi, hardware, debug, 
                        time,   
                        motorA, motorB,
                        encoderA, encoderB,
                        sr04,
                        gyro,
                        hst,
                        scheduler,
                        dataSend );

  scheduler->addCommand( commandProcessor);
  scheduler->addCommand( motorA );
  scheduler->addCommand( motorB );
  scheduler->addCommand( encoderA );
  scheduler->addCommand( encoderB );
  scheduler->addCommand( sr04 );
  scheduler->addCommand( wifi );
  auto connection = wifi->getShared();
  scheduler->addCommand( connection );
  scheduler->addCommand( hst );
  scheduler->addCommand( dataSend );
  scheduler->addCommand( gyro );
}
