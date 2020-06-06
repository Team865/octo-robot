#include <gtest/gtest.h>

#include "command_wrangler.h"

namespace FS {

TEST( COMMAND_WRANGLER, allCommandsHaveInterruptStatus)
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ++c )
  {
    ASSERT_NE( FS::CommandWrangler::doesCommandInterrupt.find( c ),
               FS::CommandWrangler::doesCommandInterrupt.end());
  }
}

TEST( COMMAND_WRANGLER, allCommandsHaveImplementations )
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ::operator++(c) )
  {
    ASSERT_NE(
      FS::CommandWrangler::commandImpl.find( c ),
      FS::CommandWrangler::commandImpl.end());
  }
}

TEST( COMMAND_WRANGLER, allStatesHaveImplementations )
{
  for ( FS::CommandState s = FS::CommandState::START_OF_STATES;
        s < FS::CommandState::END_OF_STATES; ++s )
  {
    ASSERT_NE(
      FS::CommandWrangler::stateImpl.find( s ),
      FS::CommandWrangler::stateImpl.end());
  }
}

}

TEST( SSOUND_EnUM, allStatesHaveDebugNames )
{
  for ( FS::CommandState s = FS::CommandState::START_OF_STATES;
        s < FS::CommandState::END_OF_STATES; ++s )
  {
    ASSERT_NE(
      FS::CommandStateStack::stateNames.find( s ),
      FS::CommandStateStack::stateNames.end());
  }
}



