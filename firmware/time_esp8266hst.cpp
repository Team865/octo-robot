#include <Arduino.h>
#include "time_esp8266hst.h"

namespace Time {

  DeviceTimeMS ESP8266_HST::msSinceDeviceStart()
  {
    return DeviceTimeMS(millis());
  }

  TimeUS ESP8266_HST::execute()
  {
    return Time::TimeUS( 1000000 );
  }

  const char* ESP8266_HST::debugName() 
  {
    return "ESP8266 High Speed Timer";
  }

} // end Time namespace


