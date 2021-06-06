#ifndef __COMMAND_INTERFACE_H__
#define __COMMAND_INTERFACE_H__

#include "time_types.h"

namespace Command {

///
/// @brief Base class for a command
///
/// Similar to the First Robotics Command system.  Each command
/// implements an execute function, which gives the command a slice of time
/// to do whatever it needs to do (read sensors, set GPIO pins, etc).
///
/// Unlike the First Robotics command system, execute has a return value -
/// The number of micro-seconds the scheduler should wait before it calls
/// execute again.
///
/// Because this is a co-operative system, all execute functions have to
/// quickly do their business and leave - they can't block.
/// 
class Base {
  public:

  ///
  /// @brief    Give the command "time slice"
  ///
  /// @return   The number of micro-seconds the scheduler waits before callng
  ///           execute again.
  ///
  virtual Time::TimeUS execute() = 0;

  ///
  /// @brief    Get a debug name for the command
  ///
  /// @return   The debug name.  i.e., "L298 Motor Controller"
  /// 
  virtual const char* debugName() = 0;

};

} // end namespace Command

#endif

