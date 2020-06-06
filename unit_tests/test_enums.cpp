#include <gtest/gtest.h>

#include "sample_sound.h"

TEST( SSOUND_ENUM, allCommandsHaveInterruptStatus)
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ++c )
  {
    ASSERT_NE( FS::doesCommandInterrupt.find( c ),
               FS::doesCommandInterrupt.end());
  }
}

namespace FS {

TEST( SSOUND_ENUM, allCommandsHaveImplementations )
{
  for ( CommandParser::Command c = CommandParser::Command::StartOfCommands;
        c < CommandParser::Command::EndOfCommands; ::operator++(c) )
  {
    ASSERT_NE(
      FS::SSound::commandImpl.find( c ),
      FS::SSound::commandImpl.end());
  }
}

TEST( SSOUND_ENUM, allStatesHaveImplementations )
{
  for ( FS::State s = FS::State::START_OF_STATES;
        s < FS::State::END_OF_STATES; ++s )
  {
    ASSERT_NE(
      FS::SSound::stateImpl.find( s ),
      FS::SSound::stateImpl.end());
  }
}

}

TEST( SSOUND_EnUM, allStatesHaveDebugNames )
{
  for ( FS::State s = FS::State::START_OF_STATES;
        s < FS::State::END_OF_STATES; ++s )
  {
    ASSERT_NE(
      FS::stateNames.find( s ),
      FS::stateNames.end());
  }
}



