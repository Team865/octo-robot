#include <Arduino.h>
#include "time_esp8266hst.h"

namespace 
{
  inline unsigned int readHardwareTime()
  {
    // Background, https://sub.nanona.fi/esp8266/timing-and-ticks.html
    //
    unsigned int ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));
    return ccount;
  }

} // end anonymous namespace

namespace Time {
  ESP8266_HST::ESP8266_HST()
  {
    baseHardwareTime = readHardwareTime();
    baseHardwareTimeMS = msSinceDeviceStart().get();
  }

  DeviceTimeMS ESP8266_HST::msSinceDeviceStart()
  {
    return DeviceTimeMS(millis() - baseHardwareTimeMS );
  }

  constexpr unsigned int reduce=80;

  DeviceTimeUS ESP8266_HST::usSinceDeviceStart()
  {
    const unsigned int ticksSinceBase = readHardwareTime() - baseHardwareTime; 
    const unsigned int usSinceBase = ticksSinceBase / reduce;
    return currentTime + usSinceBase;
  }

  TimeUS ESP8266_HST::execute()
  {
    // How long will a 32 bit integer incrementing at 80Mhz overflow?
    // 2^32 / 80000000 = 53 seconds
    //
    // We'll fix this by advancing baseHardwareTime every second or so, 
    // bringing the value we advance baseHardwareTime by into currentTime.
    //
    const unsigned int ticksDiff = readHardwareTime() - baseHardwareTime;
    const unsigned int usToAdvance = ticksDiff / reduce;
    const unsigned int ticksToAdvance = usToAdvance * reduce;
    currentTime = currentTime + usToAdvance;
    baseHardwareTime = baseHardwareTime + ticksToAdvance; 
    return TimeMS(1000);   // update every second
  }

  const char* ESP8266_HST::debugName() 
  {
    return "ESP8266 High Speed Timer";
  }


} // end Time namespace


