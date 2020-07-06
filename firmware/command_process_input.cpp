#include <iterator>
#include <vector>
#include <string>
#include <memory>
#include "command_parser.h"
#include "wifi_debug_ostream.h"
#include "command_process_input.h"
#include "command_scheduler.h"
#include "time_manager.h"

/////////////////////////////////////////////////////////////////////////
//
// Public Interfaces
//
/////////////////////////////////////////////////////////////////////////

namespace Command {

ProcessCommand::ProcessCommand(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<Time::Interface> timeArg,
    std::shared_ptr<Command::Motor> motorAArg,
    std::shared_ptr<Command::Motor> motorBArg,
    std::shared_ptr<Command::Encoder> encoderAArg,
    std::shared_ptr<Command::Encoder> encoderBArg,
    std::shared_ptr<Time::HST> hstArg,
    std::shared_ptr<Command::Scheduler> schedulerArg,
    std::shared_ptr<Command::DataSend> dataSendArg
) : net{ netArg }, hardware{ hardwareArg }, debugLog{ debugArg }, 
    timeMgr{ timeArg }, 
    motorA{ motorAArg }, motorB{ motorBArg }, 
    encoderA{ encoderAArg }, encoderB{ encoderBArg },
    hst{ hstArg },
    scheduler{ schedulerArg },
    dataSend{ dataSendArg }
{
  DebugInterface& dlog = *debugLog;
  dlog << "Bringing up net interface\n";
  
  WifiDebugOstream log( debugLog.get(), net->get() );

  log << "Urban-Octo-Robot is accepting commands\n";
}

Time::TimeUS ProcessCommand::execute()
{
  const Time::TimeUS uSecToNextCall = ProcessCommand::stateAcceptCommands();
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
  { CommandParser::Command::Ping,         &ProcessCommand::doPing},
  { CommandParser::Command::SetMotorA,    &ProcessCommand::doSetMotorA},
  { CommandParser::Command::SetMotorB,    &ProcessCommand::doSetMotorB},
  { CommandParser::Command::GetEncoderA,  &ProcessCommand::doGetEncoderA},
  { CommandParser::Command::GetEncoderB,  &ProcessCommand::doGetEncoderB},
  { CommandParser::Command::GetTimeMs,    &ProcessCommand::doGetTimeMs},
  { CommandParser::Command::GetTimeUs,    &ProcessCommand::doGetTimeUs},
  { CommandParser::Command::Profile,      &ProcessCommand::doProfile},
  { CommandParser::Command::RProfile,     &ProcessCommand::doRProfile},
  { CommandParser::Command::DataSend,     &ProcessCommand::doDataSend},
  { CommandParser::Command::NoCommand,    &ProcessCommand::doError},
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
  net->get() << "PONG\n";
}

void ProcessCommand::doError( CommandParser::CommandPacket cp )
{
  (void) cp;
  WifiDebugOstream log( debugLog.get(), net->get() );
  log << "ERROR!!!!\n";
}

void ProcessCommand::doSetMotorA( CommandParser::CommandPacket cp )
{
  net->get() << cp.optionalArg << "\n";
  motorA->setSpeed( cp.optionalArg );
}

void ProcessCommand::doSetMotorB( CommandParser::CommandPacket cp )
{
  net->get() << cp.optionalArg << "\n";
  motorB->setSpeed( cp.optionalArg );
}

void ProcessCommand::doGetEncoderA( CommandParser::CommandPacket cp )
{
  (void) cp;
  int position = encoderA->getPosition();
  net->get() << "encodera " << position << "\n";
}

void ProcessCommand::doGetEncoderB( CommandParser::CommandPacket cp )
{
  (void) cp;
  int position = encoderB->getPosition();
  net->get() << "encoderb " << position << "\n";
}

void ProcessCommand::doGetTimeMs( CommandParser::CommandPacket cp )
{
  (void) cp;
  net->get() << "mstimer " << hst->msSinceDeviceStart().get() << "\n";
}

void ProcessCommand::doGetTimeUs( CommandParser::CommandPacket cp )
{
  (void) cp;
  net->get() << "ustimer " << hst->usSinceDeviceStart().get() << "\n";
}

void ProcessCommand::doProfile( CommandParser::CommandPacket cp )
{
  (void) cp;
  scheduler->scheduleProfile();
}

void ProcessCommand::doRProfile( CommandParser::CommandPacket cp )
{
  (void) cp;
  net->get() << "Profile Reset\n";
  scheduler->resetProfile();
}

void ProcessCommand::doDataSend( CommandParser::CommandPacket cp )
{
  net->get() << "Datasend " << cp.optionalArg << "\n";
  dataSend->setOutput( cp.optionalArg != 0 );
}

/////////////////////////////////////////////////////////////////////////
//
// Section 3. Methods that process the commands over time
//
/////////////////////////////////////////////////////////////////////////


Time::TimeUS ProcessCommand::stateAcceptCommands()
{
  auto cp = CommandParser::checkForCommands( net->get() );

  if ( cp.command != CommandParser::Command::NoCommand )
  {
    processCommand( cp );
    return Time::TimeUS(0);
  }

  // 50 possible updates per second ( 20 ms )
  return Time::TimeMS( 1000 / 50 );
}

Time::TimeUS ProcessCommand::stateError()
{
  WifiDebugOstream log( debugLog.get(), net->get() );
  log << "hep hep hep error error error\n";
  return Time::TimeUS( 10 * Time::USPerS );
}

}

