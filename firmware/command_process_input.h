#ifndef __COMNAND_PROCESS_INPUT_H__
#define __COMNAND_PROCESS_INPUT_H__

#include <vector>
#include <memory>
#include <string>
#include <assert.h>
#include <unordered_map>

#include "command_encoder.h"
#include "command_datasend.h"
#include "command_motor.h"
#include "command_parser.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "time_interface.h"
#include "time_hst.h"

#ifdef GTEST_FOUND
#include <gtest/gtest_prod.h>
#endif

#include "command_base.h"

namespace Command
{
  class Scheduler;

/// @brief Main Processor Class
///
/// Command wrangler jobs:
/// 
/// 1.  Sends commands to individual hardware blocks
/// 2.  Gets responses from hardware blocks and sends them to the network
///
/// TODO - change name to ProcessInput
/// TODO - remove extra methods that are pass throughs
/// 
class ProcessCommand: public Base
{
  public:
 
  /// @brief ProcessCommand Constructor
  ///
  /// @param[in] netArg       - Interface to the network
  /// @param[in] hardwareArg  - Interface to the Hardware
  /// @param[in] debugArg     - Interface to the debug logger.
  /// @param[in] timeArg      - A simple time interfaced
  /// @param[in] motorAArg    - The class that controls "motor a"
  /// @param[in] encoderAArg  - The encoder for "motor a"
  /// @param[in] hstArg       - Interface for the High Speed Timer
  ///
  ProcessCommand( 
		std::shared_ptr<NetInterface> netArg,
		std::shared_ptr<HWI> hardwareArg,
		std::shared_ptr<DebugInterface> debugArg,
		std::shared_ptr<Time::Interface> timeArg,
		std::shared_ptr<Command::Motor> motorAArg,
		std::shared_ptr<Command::Encoder> encoderAArg,
		std::shared_ptr<Time::HST> hstArg, 
		std::shared_ptr<Command::Scheduler > schedulerArg,
		std::shared_ptr<Command::DataSend > dataSendArg 
	);

  ///
  /// @brief Update the Comamnd Wrangler's State
  ///
  /// @return The amount of time the caller should wait (in microseconds)
  ///         before calling loop again.
  ///
  Time::TimeUS execute() override final;

  virtual const char* debugName() override final { return "ProcessInput"; } 
  private:

#ifdef GTEST_FOUND
  // So we can unit test the consistency of the class's constant - static 
  // data without exposing it to everybody
  FRIEND_TEST(COMMAND_PROCESS_COMMAND, allCommandsHaveImplementations);
#endif

  static const std::unordered_map<CommandParser::Command,
    void (ProcessCommand::*)( CommandParser::CommandPacket),EnumHash> 
    commandImpl;

  using ptrToMember = unsigned int ( ProcessCommand::*) ( void );

  /// @brief Deleted copy constructor
  ProcessCommand( const ProcessCommand& other ) = delete;
  /// @brief Deleted default constructor
  ProcessCommand() = delete;
  /// @brief Deleted assignment operator
  ProcessCommand& operator=( const ProcessCommand& ) = delete;
  
  void processCommand( CommandParser::CommandPacket cp );

  /// @brief Wait for commands from the network interface
  Time::TimeUS stateAcceptCommands( void ); 
  /// @brief If we land in this state, complain a lot.
  Time::TimeUS stateError( void );

  void doPing( CommandParser::CommandPacket );
  void doSetMotorA( CommandParser::CommandPacket );
  void doGetEncoderA( CommandParser::CommandPacket );
  void doGetTimeMs( CommandParser::CommandPacket );
  void doGetTimeUs( CommandParser::CommandPacket );
  void doProfile( CommandParser::CommandPacket );
  void doRProfile( CommandParser::CommandPacket );
  void doDataSend( CommandParser::CommandPacket );
  void doError( CommandParser::CommandPacket );

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HWI> hardware;
  std::shared_ptr<DebugInterface> debugLog;
  std::shared_ptr<Time::Interface> timeMgr;
  
  /// @brief Interface to Motor A
  std::shared_ptr<Command::Motor> motorA;
  /// @brief Interface to Encoder A
  std::shared_ptr<Command::Encoder> encoderA;
  /// @brief Interface to the high speed timer
  std::shared_ptr<Time::HST> hst;
  /// @brief Interface to the command scheduler, for reporting
  std::shared_ptr<Command::Scheduler > scheduler;
  /// @brief Interface to the data sender, for turning on & off
  std::shared_ptr<Command::DataSend > dataSend;
 
};
}; // end namespace Command

#endif

