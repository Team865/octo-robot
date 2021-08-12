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
    std::shared_ptr<HW::I> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<Time::Interface> timeArg,
    std::shared_ptr<Command::Motor> motorLArg,
    std::shared_ptr<Command::Motor> motorRArg,
    std::shared_ptr<Command::Encoder> encoderLArg,
    std::shared_ptr<Command::Encoder> encoderRArg,
    std::shared_ptr<Command::SR04> sr04Arg,
    std::shared_ptr<Command::Gyro> gyroArg,
    std::shared_ptr<Time::HST> hstArg,
    std::shared_ptr<Command::Scheduler> schedulerArg,
    std::shared_ptr<Command::DataSend> dataSendArg
) : net{ netArg }, hardware{ hardwareArg }, debugLog{ debugArg }, 
    timeMgr{ timeArg }, 
    motorL{ motorLArg }, motorR{ motorRArg }, 
    encoderL{ encoderLArg }, encoderR{ encoderRArg },
    sr04{ sr04Arg }, gyro{ gyroArg},
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
  { CommandParser::Command::SetMotorL,    &ProcessCommand::doSetMotorL},
  { CommandParser::Command::SetMotorR,    &ProcessCommand::doSetMotorR},
  { CommandParser::Command::SetMotorA,    &ProcessCommand::doSetMotorA},
  { CommandParser::Command::GetEncoderL,  &ProcessCommand::doGetEncoderL},
  { CommandParser::Command::GetEncoderR,  &ProcessCommand::doGetEncoderR},
  { CommandParser::Command::GetTimeMs,    &ProcessCommand::doGetTimeMs},
  { CommandParser::Command::GetTimeUs,    &ProcessCommand::doGetTimeUs},
  { CommandParser::Command::Profile,      &ProcessCommand::doProfile},
  { CommandParser::Command::RProfile,     &ProcessCommand::doRProfile},
  { CommandParser::Command::DataSend,     &ProcessCommand::doDataSend},
  { CommandParser::Command::RangeSensor,  &ProcessCommand::doRangeSensor},
  { CommandParser::Command::ReadGyro,     &ProcessCommand::doReadGyro},
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

void ProcessCommand::doSetMotorL( CommandParser::CommandPacket cp )
{
  net->get() << cp.optionalArg << "\n";
  motorL->setSpeed( cp.optionalArg );
}

void ProcessCommand::doSetMotorR( CommandParser::CommandPacket cp )
{
  net->get() << cp.optionalArg << "\n";
  motorR->setSpeed( cp.optionalArg );
}

void ProcessCommand::doSetMotorA( CommandParser::CommandPacket cp )
{
  net->get() << cp.optionalArg << "\n";
  motorL->setSpeed( cp.optionalArg );
  // Flip the right motor so the robot goes forward or backwards
  motorR->setSpeed( -cp.optionalArg );
}

void ProcessCommand::doGetEncoderL( CommandParser::CommandPacket cp )
{
  (void) cp;
  int position = encoderL->getPosition();
  int rotation_speed = encoderL->getSpeed();
  net->get() << "encoderl " << position << " " << rotation_speed << "\n";
}

void ProcessCommand::doGetEncoderR( CommandParser::CommandPacket cp )
{
  (void) cp;
  int position = encoderR->getPosition();
  int rotation_speed = encoderR->getSpeed();
  net->get() << "encoderr " << position << " " << rotation_speed << "\n";
}

void ProcessCommand::doReadGyro( CommandParser::CommandPacket cp )
{
  (void) cp;
  int angle = gyro->getAngle();
  net->get() << "gyro " << angle << "\n";
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

void ProcessCommand::doRangeSensor( CommandParser::CommandPacket cp )
{
  (void) cp;
  sr04->sensorRequest();
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

