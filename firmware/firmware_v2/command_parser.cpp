#include "net_interface.h"
#include "debug_interface.h"
#include "command_parser.h"
#include "wifi_debug_ostream.h"
#include <vector>
#include <algorithm>

namespace CommandParser
{

  enum class HasArg {
    Yes,
    No
  };


/// @brief The Template for a bee-focuser command
///
/// 
class CommandTemplate
{
  public:

  const std::string inputCommand;
  CommandParser::Command outputCommand;
  const HasArg hasArg;
};

const std::vector<CommandTemplate> commandTemplates =
{
  { "ping",       Command::Ping,          HasArg::No   },
  { "motorl",     Command::SetMotorL,     HasArg::Yes  },
  { "motorr",     Command::SetMotorR,     HasArg::Yes  },
  { "motora",     Command::SetMotorA,     HasArg::Yes  },
  { "encoderl",   Command::GetEncoderL,   HasArg::No   },
  { "encoderr",   Command::GetEncoderR,   HasArg::No   },
  { "timems",     Command::GetTimeMs,     HasArg::No   },
  { "timeus",     Command::GetTimeUs,     HasArg::No   },
  { "profile",    Command::Profile,       HasArg::No   },
  { "rprofile",   Command::RProfile,      HasArg::No   },
  { "datasend",   Command::DataSend,      HasArg::Yes  },
  { "range",      Command::RangeSensor,   HasArg::No   },
  { "gyro",       Command::ReadGyro,      HasArg::No   },
}; 

/// @brief Process an integer argument
///
/// Read an integer argument from a string in a way that's guaranteed
/// not to allocate memory 
///
/// @param[in] string - The string
/// @param[in] pos    - The start position in the string.  i.e., if pos=5
///   we'll look for the number at string element 5.
/// @return           - The result.  Currently 0 if there's no number.
///
int process_int( const std::string& string,  size_t pos )
{
  size_t end = string.length();
  if ( pos > end )
    return 0;

  const bool negative = (string[pos] == '-');
  if ( negative ) ++pos;

  int result = 0;
  for ( size_t iter = pos; iter != end; iter++ ) {
    char current = string[ iter ];
    if ( current >= '0' && current <= '9' )
      result = result * 10 + ( current - '0' );
    else
      break;
  }
  return negative ? -result : result;
}

const CommandPacket checkForCommands( 
	NetConnection& connection )
{
	CommandPacket result;
  
  // Read the first line of the request.  

  static std::string command;
  command.reserve( 256 );
  bool dataReady = connection.getString( command );
  if ( !dataReady )
  {
    return result;
  }

  std::transform( command.begin(), command.end(), command.begin(), ::tolower);

  connection << "# Got: " << command << "\n";

  for ( const CommandTemplate& ct : commandTemplates )
  {
    if ( command.find(ct.inputCommand ) == 0 )
    {
      result.command = ct.outputCommand;
      if ( ct.hasArg == HasArg::Yes )
      {
        result.optionalArg =  process_int( command,  ct.inputCommand.length()+1  );
      } 
      command.resize(0);
      return result;
    }
  } 
  command.resize(0);
  return result;

}


}

