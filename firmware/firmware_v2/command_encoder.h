#ifndef __COMMAND_ENCODER_H__
#define __COMMAND_ENCODER_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"
#include "time_hst.h"

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
    std::shared_ptr<HW::I> hwiArg, 
    std::shared_ptr<DebugInterface> debugArg, 
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<Time::HST> hst,
    int i2cBusArg);

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

  ///
  /// Gets the encoder's current rotation speed
  ///
  int getSpeed();

  private:

  // @brief Interface to hardware (i.e., GPIO pins)
  std::shared_ptr<HW::I> hwi;
  // @brief Interface to debug log
  std::shared_ptr<DebugInterface> debug;
  // @brief Interface to network (i.e., Wifi)
  std::shared_ptr<NetInterface> net;
  // @brief Interface to timer
  std::shared_ptr<Time::HST> hst;

  int i2cBus;
  int position = 0;
  int last_raw_position = 0;
  int speed = 0;
  int speed_accumulate = 0;
  Time::DeviceTimeMS speed_accumulate_start;
  int speed_count = 0;
  const int I2C_ADRESS = 0x36;
  const int _raw_ang_hi = 0x0c;
  const int _raw_ang_lo = 0x0d;
};

}; // end Command namespace.

#endif

