#ifndef __COMMAND_DATASEND_H__
#define __COMMAND_DATASEND_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "command_encoder.h"
#include "command_sr04.h"
#include "command_gyro.h"
#include "debug_interface.h"
#include "hardware_interface.h"
#include "net_interface.h"

namespace Command {

///
/// @brief DataSend Controller for a L298 Controller
///
class DataSend: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] debugArg       - A debug console interface
  /// @param[in] netArg         - Interface to the WIFI network
  /// @param[in] encoderLArg    - Interface to the left motor's encoder
  /// @param[in] encoderRArg    - Interface to the right motor's encoder
  /// @param[in] rangeFinderArg - Interface to the SR04 range finder
  /// @param[in] gryoArg        - Interface to the Gyroscope
  /// @param[in] hwiArg         - Interface to the hardware, for LED setting
  /// 
  DataSend( 
    std::shared_ptr<DebugInterface>     debugArg,
    std::shared_ptr<NetInterface>       netArg,
    std::shared_ptr<Command::Encoder>   encoderLArg,
    std::shared_ptr<Command::Encoder>   encoderRArg,
    std::shared_ptr<Command::SR04>      rangeFinderArg,
    std::shared_ptr<Command::Gyro>      gyroArg,
    std::shared_ptr<HW::I>              hwiArg
  );

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
  /// @brief Start sending data every 50th of a second
  ///
  void setOutput( bool on );

  private:

  void updateLEDs();

  // @brief Interface to debug log
  std::shared_ptr<DebugInterface> debug;
  // @brief Interface to network (i.e., Wifi)
  std::shared_ptr<NetInterface> net;
  // @brief Interface to Left Motor's encoder
  std::shared_ptr<Encoder> encoderL;
  // @brief Interface to Right Motor's encoder
  std::shared_ptr<Encoder> encoderR;
  // @brief Interface to sonar range finder
  std::shared_ptr<SR04>   rangeFinder;
  // @brief Interface to gyroscope
  std::shared_ptr<Gyro>   gyro;
  // @brief Interface to hardware, for setting LEDs.
  std::shared_ptr<HW::I>  hwi;
  // @brief Are we currently outputting data
  bool isOutputting = false;
  // @brief How many times have we been called?
  unsigned int timesCalled  = 0;
  // @brief Last valid sensor reading - for LED display only.  Default = invalid
  int lastValidSensorReading = -1;
};

}; // end Command namespace.

#endif

