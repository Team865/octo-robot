#ifndef __COMMAND_ENCODER_H__
#define __COMMAND_ENCODER_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"

namespace Command {

///
/// @brief KY-040 Encoder Command
///
/// Part reference (and tutorial):  
/// https://www.handsontec.com/dataspecs/module/Rotary%20Encoder.pdf
/// 
class Encoder: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] hwiArg   - Micro-controller Pin Interface
  /// @param[in] debugArg - A debug console interface
  /// @param[in] netArg   - Interface to the WIFI network
  /// @param[in] pin0Arg  - "pin 0" on the encoder board
  /// @param[in] pin1Arg  - "pin 1" on the encoder board
  /// 
  Encoder( 
    std::shared_ptr<HWI> hwiArg, 
    std::shared_ptr<DebugInterface> debugArg, 
    std::shared_ptr<NetInterface> netArg, 
    HWI::Pin pin0Arg,  
    HWI::Pin pin1Arg);

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
  /// Gets the encoder's current position
  ///
  int getPosition();

  private:

  enum class Action {
    NO_ACTION,
    CW_ROTATION,    // clockwise
    CCW_ROTATION,   // counter clockwise rotation
    ILLEGAL,        // should never happen
  };

  unsigned int getGreyCode();

  /// @brief Action to take when we transition between grey code states.
  /// see encoder_command.cpp for details.
  ///
  static Action greyCodeActionTable[4][4];

  // @brief Interface to hardware (i.e., GPIO pins)
  std::shared_ptr<HWI> hwi;
  // @brief Interface to debug log
  std::shared_ptr<DebugInterface> debug;
  // @brief Interface to network (i.e., Wifi)
  std::shared_ptr<NetInterface> net;

  // @brief Digital input for encoder pin 0
  const HWI::Pin pin0;
  // @param Digital input for encoder pin 1
  const HWI::Pin pin1;

  unsigned int lastGreyCode;
  int position;
};

}; // end Command namespace.

#endif

