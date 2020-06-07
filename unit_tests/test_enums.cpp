#include <gtest/gtest.h>

#include "action_process_command.h"

namespace Action {

TEST( ACTION_PROCESS_COMMAND, allCommandsHaveImplementations )
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ::operator++(c) )
  {
    ASSERT_NE(
      Action::ProcessCommand::commandImpl.find( c ),
      Action::ProcessCommand::commandImpl.end());
  }
}

} // end Action namespace

