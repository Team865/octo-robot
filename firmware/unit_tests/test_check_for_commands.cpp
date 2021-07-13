#include <gtest/gtest.h>

#include "../firmware_v1/command_parser.h"
#include "test_mock_debug.h"
#include "test_mock_event.h"
#include "test_mock_hardware.h"
#include "test_mock_net.h"

namespace CommandParser
{

TEST( COMMAND_PARSER, should_process_int )
{
  ASSERT_EQ( process_int( std::string("123") , 0 ), 123 );
  ASSERT_EQ( process_int( std::string("123") , 1 ), 23 );
  ASSERT_EQ( process_int( std::string("123") , 2 ), 3 );
  ASSERT_EQ( process_int( std::string("123 "), 2 ), 3 );
  ASSERT_EQ( process_int( std::string("4567890") , 0 ), 4567890);
  ASSERT_EQ( process_int( std::string("cheese") , 0 ), 0 );
  ASSERT_EQ( process_int( std::string("ABS_POS=500") , 8 ), 500 );
  ASSERT_EQ( process_int( std::string("ABS_POS") , 8 ), 0 );
  ASSERT_EQ( process_int( std::string("ABS_POS") , 7 ), 0 );
  ASSERT_EQ( process_int( std::string("REL_POS=500") , 8 ), 500 );
  ASSERT_EQ( process_int( std::string("REL_POS=-500") , 8 ), -500 );
}

#ifdef TODO
TEST( COMMAND_PARSER, checkForCommands)
{
  DebugInterfaceIgnoreMock dbgmock;

  NetMockSimpleTimed empty;
  ASSERT_EQ( checkForCommands(dbgmock, empty), CommandPacket() );

  NetMockSimpleTimed junk("junk");
  ASSERT_EQ( checkForCommands(dbgmock, junk), CommandPacket());

  NetMockSimpleTimed abort("ping");
  ASSERT_EQ( checkForCommands(dbgmock, abort), CommandPacket( Command::Ping));

  NetMockSimpleTimed status2("Ping with training garbage");
  ASSERT_EQ( checkForCommands(dbgmock, status2), CommandPacket( Command::Ping));
}
#endif

#ifdef TODO
TEST( COMMAND_PARSER, testGot)
{
  DebugInterfaceIgnoreMock dbgmock;

  TimedStringEvents input = {
    { 0, "abort" },   // Sleep @ Time 0
  };

  // Time 0, should be a sleep
  NetMockSimpleTimed netMock( input );
  ASSERT_EQ( checkForCommands(dbgmock, netMock ), CommandPacket( Command::Abort));
  // Time 1, should be nothing
  netMock.advanceTime(1);
  ASSERT_EQ( checkForCommands(dbgmock, netMock ), CommandPacket());
  // Time 2, should be a wake.
  netMock.advanceTime(1);
 
  // Golden output - should have messages saying that the firmware
  // got the sleep and wake command
  TimedStringEvents golden = {
    { 0, "# Got: abort" }, 
  };

  // Compare.
  ASSERT_EQ( golden, netMock.getOutput() ); 
}
#endif

}

