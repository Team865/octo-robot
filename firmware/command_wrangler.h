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
#include "histogram.h"
#include "time_interface.h"
#include "state.h"
#include "action_motor.h"

#ifdef GTEST_FOUND
#include <gtest/gtest_prod.h>
#endif

#include "action_interface.h"

///
/// @brief CommandWrangler.g Namespace
/// 
namespace FS {

/// @brief CommandWrangler.g's State Enum
///
enum class CommandState 
{
  START_OF_STATES = 0,        ///< Start of States
  ACCEPT_COMMANDS = 0,        ///< Accepting commands from the net interface
  DO_PING,                    ///< Send a ping
  ERROR_STATE,                ///< Error Errror Error
  END_OF_STATES               ///< End of States
};

using CommandStateStack = StateStack< CommandState, CommandState::ACCEPT_COMMANDS >;

/// @brief Main Command Wrangler Class
///
/// Command wrangler jobs:
/// 
/// 1.  Sends commands to individual hardware blocks
/// 2.  Gets responses from hardware blocks and sends them to the network
///
class CommandWrangler: public ActionInterface
{
  public:
 
  /// @brief CommandWrangler Constructor
  ///
  /// @param[in] netArg       - Interface to the network
  /// @param[in] hardwareArg  - Interface to the Hardware
  /// @param[in] debugArg     - Interface to the debug logger.
  /// @param[in] timeArg      - A simple time interfaced
  /// @param[in] motorArg     - The class that controls "motor a"
  ///
  CommandWrangler( 
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

  virtual const char* debugName() override final { return "CommandWrangler"; } 
  private:

#ifdef GTEST_FOUND
  // So we can unit test the consistency of the class's constant - static 
  // data without exposing it to everybody
  FRIEND_TEST(COMMAND_WRANGLER, allStatesHaveImplementations );
  FRIEND_TEST(COMMAND_WRANGLER, allCommandsHaveImplementations);
  FRIEND_TEST(COMMAND_WRANGLER, allCommandsHaveInterruptStatus );
#endif

  static const std::unordered_map<CommandParser::Command,
    void (CommandWrangler::*)( CommandParser::CommandPacket),EnumHash> 
    commandImpl;

  using ptrToMember = unsigned int ( CommandWrangler::*) ( void );
  static const std::unordered_map< CommandState, ptrToMember, EnumHash > stateImpl;

  /// @brief Deleted copy constructor
  CommandWrangler( const CommandWrangler& other ) = delete;
  /// @brief Deleted default constructor
  CommandWrangler() = delete;
  /// @brief Deleted assignment operator
  CommandWrangler& operator=( const CommandWrangler& ) = delete;
  
  CommandStateStack stateStack;

  void processCommand( CommandParser::CommandPacket cp );

  static const CommandToBool doesCommandInterrupt;

  /// @brief Wait for commands from the network interface
  unsigned int stateAcceptCommands( void ); 
  /// @brief If we land in this state, complain a lot.
  unsigned int stateError( void );
  /// @brief Sending a ping request.
  unsigned int stateDoingPing( void );

  void doPing( CommandParser::CommandPacket );
  void doSetMotorA( CommandParser::CommandPacket );
  void doError( CommandParser::CommandPacket );

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HWI> hardware;
  std::shared_ptr<DebugInterface> debugLog;
  std::shared_ptr<TimeInterface> timeMgr;
  
  /// @brief CommandWrangler uptime in MS
  unsigned int time;

  /// @brief For computing time in CommandWrangler.g::loop
  unsigned int uSecRemainder;

  /// @brief Time the last command that could have caused an interrupt happened
  unsigned int timeLastInterruptingCommandOccured;

  /// @brief Interface to Motor A
  std::shared_ptr<Action::Motor> motorA;
};

};  // end fs namespace

/// @brief Increment operator for State enum
///
inline FS::CommandState& operator++( FS::CommandState& s )
{
  return BeeFocus::advance< FS::CommandState, FS::CommandState::END_OF_STATES>(s);
}


#endif

