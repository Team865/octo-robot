#ifndef __TIME_INTERFACE_H__
#define __TIME_INTERFACE_H__

#include "basic_types.h"

namespace Time
{
struct realTimeSTag{};
using RealTimeS = UrbanRobot::TypeSafeNumber< unsigned int, realTimeSTag >;

// TODO - If First Robots has a similar concept, adopt their naming scheme
class Interface {
  public:

  // TODO - should probably remove this.  On the ESP8266 it works if the
  // device is hooked up to the internet, can can't see any reason why the
  // robot needs to know what the real time is.  It's a left over from
  // other project.
  //
  virtual RealTimeS     secondsSince1970() = 0;
};
} // end Time namespace

#endif


