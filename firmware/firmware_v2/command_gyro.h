#ifndef __COMMAND_GYRO_H__
#define __COMMAND_GYRO_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"

namespace Command {
///
/// @brief Gyro Controller for a L298 Controller
///
class Gyro: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] hwiArg   - Micro-controller Pin Interface
  /// 
  Gyro( 
    std::shared_ptr<HW::I> hwiArg, 
    std::shared_ptr<DebugInterface> debugArg
  );
  Gyro() = delete;

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
  /// @brief Get the angle the robot is facing
  ///
  /// Returns the angle, in degrees, x 100.
  /// 
  unsigned getAngle();

  private:

  const std::shared_ptr<HW::I> hwi;
  const std::shared_ptr<DebugInterface> debug;

  unsigned int samples = 0;
  unsigned int angle   = 0;

};

}; // end Command namespace.

#endif

