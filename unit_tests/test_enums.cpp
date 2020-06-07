#include <gtest/gtest.h>

#include "command_wrangler.h"

TEST( COMMAND_WRANGLER, allCommandsHaveImplementations )
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ::operator++(c) )
  {
    ASSERT_NE(
      CommandWrangler::commandImpl.find( c ),
      CommandWrangler::commandImpl.end());
  }
}


