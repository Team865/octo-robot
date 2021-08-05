#ifndef __COMMAND_MOTOR_H__
#define __COMMAND_MOTOR_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"

#define _CCW 1
#define _CW 2
#define _STOP 3

namespace Command {
///
/// @brief Motor Controller for a L298 Controller
///
class Motor: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] hwiArg   - Micro-controller Pin Interface
  /// 
  Motor( std::shared_ptr<HW::I> hwiArg, std::shared_ptr<DebugInterface> debugArg, int motorNumArg);
  Motor() = delete;

  ///
  /// @brief Standard time slice function
  /// @return The number of ms the scheduler should pause the command for
  ///         after execute runs
  ///
  virtual Time::TimeUS execute() override;

  ///
  /// @brief Standard "get debug name" function
  ///
  /// @return The debug name
  ///
  virtual const char* debugName() override;

  ///
  /// @brief Set the speed of the motor.
  ///
  /// @param[in] percent  - A number from -100 to 100.  100 is full forward,
  ///     -100 is full backward, 0 is stop.
  /// 
  void setSpeed( int percent );

  private:

  // @brief Current direction of the motor - clockwise, counterclockwise, or stopped
  int dir;
  // @brief Current speed
  unsigned speedAsPercent;
  // @brief Counter. Incremented each time execute is called
  int counter;
  const std::shared_ptr<HW::I> hwi;
  const int motorNum;

  static constexpr unsigned int periodInMS = 100;
};

}; // end Command namespace.

#endif

