#include <iterator>
#include <vector>
#include <string>
#include <memory>
#include "command_parser.h"
#include "wifi_debug_ostream.h"
#include "command_wrangler.h"
#include "time_manager.h"

using namespace FS;

/////////////////////////////////////////////////////////////////////////
//
// Public Interfaces
//
/////////////////////////////////////////////////////////////////////////

CommandWrangler::CommandWrangler(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<TimeInterface> timeArg
) : net{ netArg }, hardware{ hardwareArg }, debugLog{ debugArg }, timeMgr{ timeArg }
{
  DebugInterface& dlog = *debugLog;
  dlog << "Bringing up net interface\n";
  
  // Bring up the interface to the controlling computer

  //net->setup( dlog );
  WifiDebugOstream log( debugLog.get(), net.get() );

  //hardware->PinMode(HWI::Pin::STEP,       HWI::PinIOMode::M_OUTPUT );  
 
  //hardware->DigitalWrite( HWI::Pin::DIR, HWI::PinCommandState::DIR_FORWARD); 

  log << "CommandWrangler is up\n";
}

unsigned int CommandWrangler::loop()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  ptrToMember function = stateImpl.at( stateStack.topState() );
  const unsigned uSecToNextCall = (this->*function)();
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

// What does the focuser execute if it's in a particular state?
const std::unordered_map<CommandState,unsigned int (CommandWrangler::*)( void ),EnumHash>
  CommandWrangler::stateImpl =
{
  { CommandState::ACCEPT_COMMANDS,           &CommandWrangler::stateAcceptCommands },
  { CommandState::DO_PING,                   &CommandWrangler::stateDoingPing},
  { CommandState::ERROR_STATE,               &CommandWrangler::stateError }
};

// Bind CommandState Enums to Human Readable Debug Names
template<>
const CommandStateStack::StateToString CommandStateStack::stateNames =
{
  { CommandState::ACCEPT_COMMANDS,               "ACCEPTING_COMMANDS" },
  { CommandState::DO_PING,                       "SENDING_PING" },
  { CommandState::ERROR_STATE,                   "ERROR ERROR ERROR"  },
};

// Implementation of the commands that the CommandWrangler Supports 
const std::unordered_map<CommandParser::Command,
  void (CommandWrangler::*)( CommandParser::CommandPacket),EnumHash> 
  CommandWrangler::commandImpl = 
{
  { CommandParser::Command::Ping,       &CommandWrangler::doPing},
  { CommandParser::Command::NoCommand,  &CommandWrangler::doError},
};

// Can a command be interrupted/aborted?
const CommandToBool CommandWrangler::doesCommandInterrupt= 
{
  { CommandParser::Command::Ping,          false  },
  { CommandParser::Command::NoCommand,     false  },
};

/////////////////////////////////////////////////////////////////////////
//
// Section 2. Methods that interpret input from the network
//
/////////////////////////////////////////////////////////////////////////

// Entry point for all commands
void CommandWrangler::processCommand( CommandParser::CommandPacket cp )
{
  if ( doesCommandInterrupt.at( cp.command ))
  {
    timeLastInterruptingCommandOccured = time;
  }
  auto function = commandImpl.at( cp.command );
  (this->*function)( cp );
}

void CommandWrangler::doError( CommandParser::CommandPacket cp )
{
  (void) cp;
  stateStack.push( CommandState::ERROR_STATE, __LINE__ );   
}

void CommandWrangler::doPing( CommandParser::CommandPacket cp )
{
  (void) cp;
  WifiDebugOstream log( debugLog.get(), net.get() );
  stateStack.push( CommandState::DO_PING, __LINE__ ); 
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

  return 1000*1000;
}

unsigned int CommandWrangler::stateDoingPing()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "PONG\n";
  stateStack.pop();
  
  return 1000*1000;
}

unsigned int CommandWrangler::stateError()
{
  WifiDebugOstream log( debugLog.get(), net.get() );
  log << "hep hep hep error error error\n";
  return 10*1000*1000; // 10 sec pause 
}


