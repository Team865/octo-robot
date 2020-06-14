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
    virtual Time::DeviceTimeUS usSinceDeviceStart() override final;
    virtual Time::TimeUS execute() override final;
    virtual const char* debugName() override final;

    private:
      Time::DeviceTimeUS currentTime;
      unsigned int baseHardwareTime;
      unsigned int baseHardwareTimeMS;
  };
}

#endif

