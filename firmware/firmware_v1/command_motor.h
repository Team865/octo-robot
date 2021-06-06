#ifndef __COMMAND_MOTOR_H__
#define __COMMAND_MOTOR_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"

namespace Command {

///
/// @brief The interface to the Motor hardware
/// 
class MotorHardware {

  public:

  /// @brief Constructor
  ///
  /// @param[in] hwiArg   - Micro-controller Pin Interface
  /// @param[in] pin0Arg  - Digital Pin that controls one of the motor inputs 
  /// @param[in] pin1Arg  - Digital Pin that controls the other motor input
  ///
  MotorHardware( 
    std::shared_ptr<HW::I> hwiArg, 
    HW::Pin pin0Arg,  
    HW::Pin pin1Arg);
  MotorHardware() = delete;

  /// @brief Tell the motor to go forward
  void setMotorForward();
  /// @brief Tell the motor to go backward
  void setMotorBackward();
  /// @brief Tell the motor to stop
  void setMotorStop();
  
  private:

  // @brief Interface to hardware (i.e., GPIO pins)
  std::shared_ptr<HW::I> hwi;
  // @brief Digital Pin that controls one of the motor inputs 
  const HW::Pin pin0;
  // @param Digital Pin that controls the other motor input
  const HW::Pin pin1;
};

///
/// @brief Manages the Motor's state
/// 
class MotorState
{
  public:
  MotorState( 
    std::shared_ptr<HW::I> hwiArg, 
    HW::Pin pin0Arg,  
    HW::Pin pin1Arg);
  MotorState() = delete;

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
  // @brief Set the motor controller output, but only if it's changed.
  //
  // Compares the desired motor controller output to the current motor
  // output and calls doPulse if the desired output is different.
  //
  // @param[in] Pulse  -  Desired new output value
  //
  void doPulse( Pulse pulse );

  private:

  // @brief Interface to the physical motor hardware
  MotorHardware motorHardware;
  // @brief What was the motor last set to?
  Pulse lastPulse;
};

///
/// @brief Motor Controller for a L298 Controller
///
class Motor: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] hwiArg   - Micro-controller Pin Interface
  /// @param[in] pin0Arg  - Digital Pin that controls one of the motor inputs 
  /// @param[in] pin1Arg  - Digital Pin that controls the other motor input
  /// 
  Motor( 
    std::shared_ptr<HW::I> hwiArg, 
    HW::Pin pin0Arg,  
    HW::Pin pin1Arg);
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

  //
  // @brief What direction is the motor going? 
  //
  enum class Dir {
    FORWARD,
    BACKWARDS
  };
  
  // @brief The state of the motor
  MotorState motorState;

  // @brief Current direction of the motor - forwards or backwards
  Dir dir;
  // @brief Current speed
  unsigned speedAsPercent;
  // @brief Counter. Incremented each time execute is called
  int counter;

  static constexpr unsigned int periodInMS = 100;
};

}; // end Command namespace.

#endif

