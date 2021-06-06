#ifndef __TIME_ESP8266HST_H__
#define __TIME_ESP8266HST_H__

#include "time_hst.h"

namespace Time 
{
  ///
  /// @brief ESP8266 High Speed Timer concept
  ///
  class ESP8266_HST: public HST {
    public:

    ESP8266_HST();
    ~ESP8266_HST() {}

    virtual Time::DeviceTimeMS msSinceDeviceStart() override final;
    virtual Time::DeviceTimeUS ICACHE_RAM_ATTR usSinceDeviceStart() override final
    {
      const unsigned int ticksSinceBase = readHardwareTime() - baseHardwareTime;
      const unsigned int usSinceBase = ticksSinceBase / reduce;
      return currentTime + usSinceBase;
    }
    virtual Time::TimeUS execute() override final;
    virtual const char* debugName() override final;

    private:
      Time::DeviceTimeUS currentTime;
      unsigned int baseHardwareTime;
      unsigned int baseHardwareTimeMS;
      unsigned int ICACHE_RAM_ATTR readHardwareTime() 
      {
        // Background, https://sub.nanona.fi/esp8266/timing-and-ticks.html
        //
        unsigned int ccount;
        asm volatile ("rsr %0, ccount" : "=r"(ccount));
        return ccount;
      }
      static constexpr unsigned int reduce=80;
  };
}

#endif

