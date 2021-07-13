#include <gtest/gtest.h>

#include "../firmware_v1/command_process_input.h"

namespace Command {

TEST( COMMAND_PROCESS_COMMAND, allCommandsHaveImplementations )
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ::operator++(c) )
  {
    ASSERT_NE(
      Command::ProcessCommand::commandImpl.find( c ),
      Command::ProcessCommand::commandImpl.end());
  }
}

} // end Action namespace

