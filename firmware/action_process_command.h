#ifndef __COMMAND_WRANGER_H__
#define __COMMAND_WRANGER_H__

#include <vector>
#include <memory>
#include <string>
#include <assert.h>
#include <unordered_map>
#include "command_parser.h"
#include "net_interface.h"
#include "hardware_interface.h"
#include "time_interface.h"
#include "action_motor.h"

#ifdef GTEST_FOUND
#include <gtest/gtest_prod.h>
#endif

#include "action_interface.h"

namespace Action
{

/// @brief Main Command Wrangler Class
///
/// Command wrangler jobs:
/// 
/// 1.  Sends commands to individual hardware blocks
/// 2.  Gets responses from hardware blocks and sends them to the network
///
class ProcessCommand: public Interface
{
  public:
 
  /// @brief ProcessCommand Constructor
  ///
  /// @param[in] netArg       - Interface to the network
  /// @param[in] hardwareArg  - Interface to the Hardware
  /// @param[in] debugArg     - Interface to the debug logger.
  /// @param[in] timeArg      - A simple time interfaced
  /// @param[in] motorArg     - The class that controls "motor a"
  ///
  ProcessCommand( 
		std::shared_ptr<NetInterface> netArg,
		std::shared_ptr<HWI> hardwareArg,
		std::shared_ptr<DebugInterface> debugArg,
		std::shared_ptr<TimeInterface> timeArg,
		std::shared_ptr<Action::Motor> motorArg
	);

  ///
  /// @brief Update the Comamnd Wrangler's State
  ///
  /// @return The amount of time the caller should wait (in microseconds)
  ///         before calling loop again.
  ///
  virtual unsigned int loop() override final;

  virtual const char* debugName() override final { return "ProcessCommand"; } 
  private:

#ifdef GTEST_FOUND
  // So we can unit test the consistency of the class's constant - static 
  // data without exposing it to everybody
  FRIEND_TEST(ACTION_PROCESS_COMMAND, allCommandsHaveImplementations);
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
  unsigned int stateAcceptCommands( void ); 
  /// @brief If we land in this state, complain a lot.
  unsigned int stateError( void );

  void doPing( CommandParser::CommandPacket );
  void doSetMotorA( CommandParser::CommandPacket );
  void doError( CommandParser::CommandPacket );

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HWI> hardware;
  std::shared_ptr<DebugInterface> debugLog;
  std::shared_ptr<TimeInterface> timeMgr;
  
  /// @brief ProcessCommand uptime in MS
  unsigned int time;

  /// @brief For computing time in CommandWranger::loop
  unsigned int uSecRemainder;

  /// @brief Interface to Motor A
  std::shared_ptr<Action::Motor> motorA;
};
}; // end namespace action

#endif

