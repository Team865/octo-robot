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
#include "command_sr04.h"
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
  /// @param[in] motorLArg    - The class that controls the left motor
  /// @param[in] motorRArg    - The class that controls the right motor
  /// @param[in] encoderLArg  - The encoder for left motor 
  /// @param[in] encoderRArg  - The encoder for right motor
  /// @param[in] sr04Arg      - The s404 sonar range finder
  /// @param[in] hstArg       - Interface for the High Speed Timer
  /// @param[in] schedulerArg - The schedululer.  Used to display profiling
  /// @param[in] dataSendArg  - Sends data to the host every 1/50 sec
  ///
  ProcessCommand( 
		std::shared_ptr<NetInterface> netArg,
		std::shared_ptr<HW::I> hardwareArg,
		std::shared_ptr<DebugInterface> debugArg,
		std::shared_ptr<Time::Interface> timeArg,
		std::shared_ptr<Command::Motor> motorLArg,
		std::shared_ptr<Command::Motor> motorRArg,
		std::shared_ptr<Command::Encoder> encoderLArg,
		std::shared_ptr<Command::Encoder> encoderRArg,
		std::shared_ptr<Command::SR04> sr04Arg,
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
  void doSetMotorL( CommandParser::CommandPacket );
  void doSetMotorR( CommandParser::CommandPacket );
  void doSetMotorA( CommandParser::CommandPacket );
  void doGetEncoderL( CommandParser::CommandPacket );
  void doGetEncoderR( CommandParser::CommandPacket );
  void doGetTimeMs( CommandParser::CommandPacket );
  void doGetTimeUs( CommandParser::CommandPacket );
  void doProfile( CommandParser::CommandPacket );
  void doRProfile( CommandParser::CommandPacket );
  void doDataSend( CommandParser::CommandPacket );
  void doRangeSensor( CommandParser::CommandPacket );
  void doError( CommandParser::CommandPacket );

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HW::I> hardware;
  std::shared_ptr<DebugInterface> debugLog;
  std::shared_ptr<Time::Interface> timeMgr;
  
  /// @brief Interface to the Left Motor 
  std::shared_ptr<Command::Motor> motorL;
  /// @brief Interface to the Right Motor 
  std::shared_ptr<Command::Motor> motorR;
  /// @brief Interface to the Left Encoder 
  std::shared_ptr<Command::Encoder> encoderL;
  /// @brief Interface to the Right Encoder
  std::shared_ptr<Command::Encoder> encoderR;
  /// @brief Interface to SR04 Sonar Range Finder
  std::shared_ptr<Command::SR04> sr04;
  /// @brief Interface to the high speed timer
  std::shared_ptr<Time::HST> hst;
  /// @brief Interface to the command scheduler, for reporting
  std::shared_ptr<Command::Scheduler > scheduler;
  /// @brief Interface to the data sender, for turning on & off
  std::shared_ptr<Command::DataSend > dataSend;
 
};
}; // end namespace Command

#endif

