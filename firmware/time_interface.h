#ifndef __TIME_INTERFACE_H__
#define __TIME_INTERFACE_H__

#include "basic_types.h"

namespace Time
{
  /// @brief A unit of time measured in ms since the device started.

  extern const char devTimeLabel[];
  using DeviceTimeMS = UrbanRobot::TypeSafeNumber< unsigned int, devTimeLabel >;

class Interface {
  public:

  virtual unsigned int secondsSince1970() = 0;
  virtual DeviceTimeMS msSinceDeviceStart() = 0;
};

} // end Time namespace

#endif


