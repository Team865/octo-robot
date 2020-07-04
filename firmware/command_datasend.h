#ifndef __COMMAND_DATASEND_H__
#define __COMMAND_DATASEND_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "command_encoder.h"
#include "net_interface.h"
#include "debug_interface.h"

namespace Command {

///
/// @brief DataSend Controller for a L298 Controller
///
class DataSend: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] debugArg - A debug console interface
  /// @param[in] netArg   - Interface to the WIFI network
  /// 
  DataSend( 
    std::shared_ptr<DebugInterface>     debugArg,
    std::shared_ptr<NetInterface>       netArg,
    std::shared_ptr<Command::Encoder>   encoderAArg
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

  bool isOutputting;
  std::shared_ptr<Encoder> encoderA;
  // @brief Interface to debug log
  std::shared_ptr<DebugInterface> debug;
  // @brief Interface to network (i.e., Wifi)
  std::shared_ptr<NetInterface> net;
};

}; // end Command namespace.

#endif

