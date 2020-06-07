#include <iterator>
#include <vector>
#include <string>
#include <memory>
#include "command_parser.h"
#include "wifi_debug_ostream.h"
#include "command_wrangler.h"
#include "time_manager.h"

/////////////////////////////////////////////////////////////////////////
//
// Public Interfaces
//
/////////////////////////////////////////////////////////////////////////

CommandWrangler::CommandWrangler(
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

unsigned int CommandWrangler::loop()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  const unsigned uSecToNextCall = CommandWrangler::stateAcceptCommands();
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

// Implementation of the commands that the CommandWrangler Supports 
const std::unordered_map<CommandParser::Command,
  void (CommandWrangler::*)( CommandParser::CommandPacket),EnumHash> 
  CommandWrangler::commandImpl = 
{
  { CommandParser::Command::Ping,       &CommandWrangler::doPing},
  { CommandParser::Command::SetMotorA,  &CommandWrangler::doSetMotorA},
  { CommandParser::Command::NoCommand,  &CommandWrangler::doError},
};

/////////////////////////////////////////////////////////////////////////
//
// Section 2. Methods that interpret input from the network
//
/////////////////////////////////////////////////////////////////////////

// Entry point for all commands
void CommandWrangler::processCommand( CommandParser::CommandPacket cp )
{
  auto function = commandImpl.at( cp.command );
  (this->*function)( cp );
}

void CommandWrangler::doPing( CommandParser::CommandPacket cp )
{
  (void) cp;
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "PONG\n";
}

void CommandWrangler::doError( CommandParser::CommandPacket cp )
{
  (void) cp;
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "ERROR!!!!\n";
}

void CommandWrangler::doSetMotorA( CommandParser::CommandPacket cp )
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


unsigned int CommandWrangler::stateAcceptCommands()
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

unsigned int CommandWrangler::stateError()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "hep hep hep error error error\n";
  return 10*1000*1000; // 10 sec pause 
}


