#include <iterator>
#include <vector>
#include <string>
#include <memory>
#include "command_parser.h"
#include "wifi_debug_ostream.h"
#include "action_process_command.h"
#include "time_manager.h"

/////////////////////////////////////////////////////////////////////////
//
// Public Interfaces
//
/////////////////////////////////////////////////////////////////////////

namespace Action {

ProcessCommand::ProcessCommand(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<TimeInterface> timeArg,
    std::shared_ptr<Action::Motor> motorAArg
) : net{ netArg }, hardware{ hardwareArg }, debugLog{ debugArg }, timeMgr{ timeArg }, motorA{ motorAArg }
{
  DebugInterface& dlog = *debugLog;
  dlog << "Bringing up net interface\n";
  
  WifiDebugOstream log( debugLog.get(), net.get() );

  log << "Urban-Octo-Robot is accepting commands\n";
}

unsigned int ProcessCommand::loop()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  const unsigned uSecToNextCall = ProcessCommand::stateAcceptCommands();
  uSecRemainder += uSecToNextCall;
  time += uSecRemainder / 1000;
  uSecRemainder = uSecRemainder % 1000;
  net->flush();
  return uSecToNextCall;
}

/////////////////////////////////////////////////////////////////////////
//
// Private Interfaces
//
// Section Overview:
// 
// 1. Constant Data
// 2. Methods that interpret input from the network
// 3. Methods that process the commands over time
// 4. Utility Methods
// 
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
// Section 1.  Constant Data
//
/////////////////////////////////////////////////////////////////////////

// Implementation of the commands that the ProcessCommand Supports 
const std::unordered_map<CommandParser::Command,
  void (ProcessCommand::*)( CommandParser::CommandPacket),EnumHash> 
  ProcessCommand::commandImpl = 
{
  { CommandParser::Command::Ping,       &ProcessCommand::doPing},
  { CommandParser::Command::SetMotorA,  &ProcessCommand::doSetMotorA},
  { CommandParser::Command::NoCommand,  &ProcessCommand::doError},
};

/////////////////////////////////////////////////////////////////////////
//
// Section 2. Methods that interpret input from the network
//
/////////////////////////////////////////////////////////////////////////

// Entry point for all commands
void ProcessCommand::processCommand( CommandParser::CommandPacket cp )
{
  auto function = commandImpl.at( cp.command );
  (this->*function)( cp );
}

void ProcessCommand::doPing( CommandParser::CommandPacket cp )
{
  (void) cp;
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "PONG\n";
}

void ProcessCommand::doError( CommandParser::CommandPacket cp )
{
  (void) cp;
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "ERROR!!!!\n";
}

void ProcessCommand::doSetMotorA( CommandParser::CommandPacket cp )
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << cp.optionalArg << "\n";
  motorA->setSpeed( cp.optionalArg );
}



/////////////////////////////////////////////////////////////////////////
//
// Section 3. Methods that process the commands over time
//
/////////////////////////////////////////////////////////////////////////


unsigned int ProcessCommand::stateAcceptCommands()
{
  DebugInterface& log = *debugLog;
  auto cp = CommandParser::checkForCommands( log, *net );

  if ( cp.command != CommandParser::Command::NoCommand )
  {
    processCommand( cp );
    return 0;
  }

  // 50 possible updates per second.
  return 20*1000;
}

unsigned int ProcessCommand::stateError()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "hep hep hep error error error\n";
  return 10*1000*1000; // 10 sec pause 
}

}

