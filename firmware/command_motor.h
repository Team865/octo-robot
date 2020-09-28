#ifndef __COMMAND_MOTOR_H__
#define __COMMAND_MOTOR_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"

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
  /// @param[in] debugArg - A debug console interface
  /// @param[in] netArg   - Interface to the WIFI network
  /// @param[in] pin0Arg  - Digital Pin that controls one of the motor inputs 
  /// @param[in] pin1Arg  - Digital Pin that controls the other motor input
  /// 
  Motor( 
    std::shared_ptr<HW::I> hwiArg, 
    std::shared_ptr<DebugInterface> debugArg, 
    std::shared_ptr<NetInterface> netArg, 
    HW::Pin pin0Arg,  
    HW::Pin pin1Arg);

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

  //
  // @brief What direction is the motor going? 
  //
  enum class Dir {
    FORWARD,
    BACKWARDS
  };
  
  // @brief What kind of output are we sending to the controller right now?>
  //
  // FORWARD  - 100% Forward 
  // BACKWARD - 100% Backward
  // NONE     - Motor is powered down
  //
  enum class Pulse {
    FORWARD,
    BACKWARD,
    NONE
  };

  // 
  // @brief Set the motor controller output as per pulse
  //
  // @param[in] Pulse  -  New output value
  //
  void doPulse( Pulse pulse );
  
  //
  // @brief Set the motor controller output, but only if it's changed.
  //
  // Compares the desired motor controller output to the current motor
  // output and calls doPulse if the desired output is different.
  //
  // Useful for debugging using unit tests or the firmware simulator
  //
  // @param[in] Pulse  -  Desired new output value
  //
  void doPulseIfChanged( Pulse pulse );

  // @brief Interface to hardware (i.e., GPIO pins)
  std::shared_ptr<HW::I> hwi;
  // @brief Interface to debug log
  std::shared_ptr<DebugInterface> debug;
  // @brief Interface to network (i.e., Wifi)
  std::shared_ptr<NetInterface> net;
  // @brief Digital Pin that controls one of the motor inputs 
  const HW::Pin pin0;
  // @param Digital Pin that controls the other motor input
  const HW::Pin pin1;

  // @brief Current direction of the motor - forwards or backwards
  Dir dir;
  // @brief Current speed
  unsigned speedAsPercent;
  // @brief Counter. Incremented each time execute is called
  int counter;
  // @brief What was the motor last set to?
  Pulse lastPulse;

  static constexpr unsigned int periodInMS = 100;
};

}; // end Command namespace.

#endif

