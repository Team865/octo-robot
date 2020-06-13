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

    virtual Time::DeviceTimeMS msSinceDeviceStart() override final;
    virtual Time::TimeUS execute() override final;
    virtual const char* debugName() override final;
  };
}

#endif

