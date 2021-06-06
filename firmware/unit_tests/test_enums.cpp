#include <gtest/gtest.h>

#include "command_process_input.h"

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

