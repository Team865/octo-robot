#ifndef __TIME_INTERFACE_H__
#define __TIME_INTERFACE_H__

#include "basic_types.h"

namespace Time
{
  /// @brief A unit of time measured in ms since the device started.

  extern const char deviceTimeMSTag[];
  using DeviceTimeMS = UrbanRobot::TypeSafeNumber< unsigned int, deviceTimeMSTag >;
  extern const char realTimeSTag[];
  using RealTimeS = UrbanRobot::TypeSafeNumber< unsigned int, realTimeSTag >;

class Interface {
  public:

  virtual RealTimeS     secondsSince1970() = 0;
  virtual DeviceTimeMS  msSinceDeviceStart() = 0;
};

} // end Time namespace

#endif


