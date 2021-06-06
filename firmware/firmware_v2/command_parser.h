#ifndef __COMMAND_PARSER_H__
#define __COMMAND_PARSER_H__

#include "basic_types.h"
#include "hardware_interface.h"
#include "debug_interface.h"

class NetConnection;

namespace CommandParser {

  int process_int( const std::string& string,  size_t pos );

  enum class Command {
    StartOfCommands = 0,  ///<  Start of the command list
    Ping            = 0,  ///<  Send a pong
    SetMotorL,            ///<  Set left motor to a value
    SetMotorR,            ///<  Set right motor to a value
    SetMotorA,            ///<  Set both motors to a value (for debugging)
    GetEncoderL,          ///<  Get the current left encoder value
    GetEncoderR,          ///<  Get the current right encoder value
    GetTimeMs,            ///<  Get ms since device start
    GetTimeUs,            ///<  Get us since device start
    Profile,              ///<  Dump profiling data to net
    RProfile,             ///<  Reset profiling data
    DataSend,             ///<  If arg=1, send state data 50x / sec. arg=0 stops
    RangeSensor,          ///<  Read the SR04 range sensor
    NoCommand,            ///<  No command was specified.
    EndOfCommands         ///<  End of the comand list.
  };

  constexpr int NoArg = -1;

  class CommandPacket  {
    public:
    CommandPacket(): command{Command::NoCommand}, optionalArg{NoArg}
    {
    }
    CommandPacket( Command c ): command{c}, optionalArg{NoArg}
    {
    }
    CommandPacket( Command c, int o ): command{c}, optionalArg{o}
    {
    }

    bool operator==( const CommandPacket &rhs ) const 
    {
      return rhs.command == command && rhs.optionalArg == optionalArg;
    }

    Command command;
    int optionalArg;
  };

  /// @brief Get commands from the network interface
  ///
  /// @param[in] log          - Debug Log stream
  /// @param[in] netInterface - The network interface that we'll query
  ///            for the command.
  /// @return    New requests from netInterface that need to be acted
  ///            on.
  ///
  /// TODO 
  /// - Error handling (has none).
  /// - Move extra parameters used by the STATUS command.
  ///
  const CommandPacket checkForCommands( 
    NetConnection& netInterface	// Input: Network Connection
  );

};

/// @brief Increment operator for Command enum
///
inline CommandParser::Command& operator++( CommandParser::Command &c )
{
  return UrbanRobot::advance< CommandParser::Command, CommandParser::Command::EndOfCommands >(c);
}

using CommandToBool = std::unordered_map< CommandParser::Command, bool, EnumHash >;

#endif

