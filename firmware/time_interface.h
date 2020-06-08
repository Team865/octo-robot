#ifndef __TIME_INTERFACE_H__
#define __TIME_INTERFACE_H__

#include "basic_types.h"

namespace Time
{
/// @brief A unit of time measured in ms since the device started.

///
/// TODO - change times to unsigned int, especially millisecond times.
/// Fun fact, a 32 bit signed millisecond variable will overflow every
/// 24 days.  a 32 bit microsecond variable will overflow every in 35
/// minutes (signed) or 70 minutes (unsigned).
///
struct deviceTimeMSTag{};
using DeviceTimeMS = UrbanRobot::TypeSafeNumber< unsigned int, deviceTimeMSTag >;

struct timeMSTag{};
using TimeMS = UrbanRobot::TypeSafeNumber< unsigned int, timeMSTag >;

struct realTimeSTag{};
using RealTimeS = UrbanRobot::TypeSafeNumber< unsigned int, realTimeSTag >;

struct deviceTimeUSTag{};
using DeviceTimeUS = UrbanRobot::TypeSafeNumber< unsigned long long, deviceTimeUSTag>;

struct timeUSTag{};
using TimeUS = UrbanRobot::TypeSafeNumber< unsigned long long, timeUSTag>;

// TODO - If First Robots has a similar concept, adopt their naming scheme
class Interface {
  public:

  // TODO - should probably remove this.  On the ESP8266 it works if the
  // device is hooked up to the internet, can can't see any reason why the
  // robot needs to know what the real time is.  It's a left over from
  // other project.
  //
  virtual RealTimeS     secondsSince1970() = 0;

  //
  // @brief Get the number of milliseconds since the device was turned on
  // 
  // @return Time "on" time in milliseconds
  // 
  virtual DeviceTimeMS  msSinceDeviceStart() = 0;
};

inline TimeMS UStoMS( TimeUS in ) { return TimeMS( in.get() / 1000 ); }
inline TimeUS MStoUS( TimeMS in ) { return TimeUS( in.get() * 1000 ); }

constexpr unsigned int USPerMs = 1000;
constexpr unsigned int USPerS = 1000000;

} // end Time namespace

inline Time::DeviceTimeUS operator+( Time::DeviceTimeUS lhs, Time::TimeUS rhs ) {
  return lhs + rhs.get();
}

inline Time::DeviceTimeUS operator+( Time::TimeUS lhs, Time::DeviceTimeUS rhs ) {
  return rhs + lhs.get();
}



#endif


