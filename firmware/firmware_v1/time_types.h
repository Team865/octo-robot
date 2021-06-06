#ifndef __TIME_TYPES_H__
#define __TIME_TYPES_H__

#include "basic_types.h"

namespace Time 
{
// tags to uniquely identify time units
struct deviceTimeMSTag{};
struct timeMSTag{};
struct deviceTimeUSTag{};
struct timeUSTag{};

/// @brief A unit of time measured in ms since the device started.
using DeviceTimeMS = UrbanRobot::TypeSafeNumber< unsigned int, deviceTimeMSTag >;

/// @brief A unit of time measured in ms 
using TimeMS = UrbanRobot::TypeSafeNumber< unsigned int, timeMSTag >;

/// @brief A unit of time measured in us since the device started.
using DeviceTimeUS = UrbanRobot::TypeSafeNumber< unsigned long long, deviceTimeUSTag>;

// @brief Helper for TimeUS
using TimeUSRaw = UrbanRobot::TypeSafeNumber< unsigned long long, timeUSTag>;

///
/// @brief A unit of time measured in micro-seconds 
/// 
/// Proof of concept - wraps a typesafe int class and adds conversions
///
class TimeUS: public TimeUSRaw {
  public:
  // Pass through constructors
  explicit TimeUS( TimeUSRaw::BaseNumber num ) : TimeUSRaw{ num } {}
  TimeUS() = default;
  TimeUS( const TimeUSRaw& rhs ) : TimeUSRaw{ rhs } {};

  // New conversion constructors
  TimeUS( const TimeMS& rhs ) : TimeUSRaw{ rhs.get() * 1000 } {}
};

inline TimeMS UStoMS( TimeUS in ) { return TimeMS( in.get() / 1000 ); }
inline TimeUS MStoUS( TimeMS in ) { return TimeUS( in.get() * 1000 ); }

constexpr unsigned int USPerMs = 1000;
constexpr unsigned int USPerS = 1000000;

} // End namespace Time

inline Time::DeviceTimeUS operator+( Time::DeviceTimeUS lhs, Time::TimeUS rhs ) {
  return lhs + rhs.get();
}

inline Time::DeviceTimeUS operator+( Time::TimeUS lhs, Time::DeviceTimeUS rhs ) {
  return rhs + lhs.get();
}


#endif

