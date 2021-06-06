#ifndef __TIME_HST_H__
#define __TIME_HST_H__

#include "basic_types.h"
#include "time_types.h"
#include "command_base.h"

namespace Time 
{
///
/// @brief High Speed Timer concept
///
/// Why do we need a high speed time?
///
/// On something like a First competition robot, you could expect a lot of
/// what we're doing on the micro-controller to be off-loaded to peripherals
/// The processor doesn't have to watch the encoders for state changes -
/// there's hardware on the encoder to do that.
///
/// Here, the processor is doing all the work, so it can't miss an event 
/// like an encoder start change.
///
/// Could we use interrupts instead of leaning on Command/ Execute to
/// respond to events?  Maybe.  A lot of people seem to have problems with
/// interrupts on the ESP8266 and random crashes, possibly from a bad
/// interraction with the interrupts that are servicing the network stack.
/// So, maybe an option, but if can be make to work without them, KISS.
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
virtual DeviceTimeUS usSinceDeviceStart() = 0;
};
} // End namespace Time

#endif

