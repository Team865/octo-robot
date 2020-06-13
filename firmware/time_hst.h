#ifndef __TIME_HST_H__
#define __TIME_HST_H__

#include "basic_types.h"
#include "command_base.h"

namespace Time 
{
  ///
  /// @brief High Speed Timer concept
  ///
  class HST: public Command::Base {
    public:

    //
    // @brief Get the number of milliseconds since the device was turned on
    // 
    // @return Time "on" time in milliseconds
    // 
    virtual Time::DeviceTimeMS msSinceDeviceStart() = 0;

    //
    // @brief Get the number of microseconds since the device was turned on
    // 
    // @return Time "on" time in microseconds
    // 
    // virtual DeviceTimeUS usSinceDeviceStart() = 0;
  };
} // End namespace Time

#endif

