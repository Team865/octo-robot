#include <gtest/gtest.h>

/// 
/// @brief Tests for Util::IPinEvents
///

#include "util_ipinevents.h"

namespace Util {

//
// Repeatedly adds and removes events
// 
TEST( pipe_should, add_and_remove_events )
{
  IPinEvents<16> events;

  const std::vector<IPinEvent> golden = {
    { HW::PinState::INPUT_HIGH, Time::DeviceTimeUS{1} },
    { HW::PinState::INPUT_LOW,  Time::DeviceTimeUS{2} },
    { HW::PinState::INPUT_HIGH, Time::DeviceTimeUS{3} }
  };

  // 100 loops should be a good stress test
  for ( unsigned i = 0; i < 100; ++i )
  {
    // starts empty
    ASSERT_EQ( false, events.hasEvents() );
 
    // Add events
    for ( const auto& event : golden ) 
    {
      events.write( event );
    }

    // Should no longer be empty
    ASSERT_EQ( true,  events.hasEvents() );

    // Remove events and make sure they're the right events
    for ( const auto& event : golden ) 
    {
      ASSERT_EQ( event, events.read() );
    }

    // Tests.  Finishes empty & with no errors.
    ASSERT_EQ( false, events.hasEvents() );
    ASSERT_EQ( false, events.hasReadError() );
    ASSERT_EQ( false, events.hasWriteError() );
  }
}

//
// Repeatedly write until the buffer is full and then drain
//
TEST( pipe_should, write_to_full )
{
  IPinEvents<4> events;

  // Full is one less than the max (3) under the current implementation
  //
  const std::vector<IPinEvent> golden = {
    { HW::PinState::INPUT_HIGH, Time::DeviceTimeUS{1} },
    { HW::PinState::INPUT_LOW,  Time::DeviceTimeUS{2} },
    { HW::PinState::INPUT_HIGH, Time::DeviceTimeUS{3} }
  };

  // 100 = stress test
  for( unsigned i = 0; i < 100; ++i ) {
    // Starts empty
    ASSERT_EQ( false, events.hasEvents() );
 
    // Fill to max
    for ( const auto& event : golden ) {
      events.write( event );
    }

    // No longer empty
    ASSERT_EQ( true,  events.hasEvents() );

    // Read events
    for ( const auto& event : golden ) {
      ASSERT_EQ( event, events.read() );
    }

    // Should be empty & with no errors
    ASSERT_EQ( false, events.hasEvents() );
    ASSERT_EQ( false, events.hasReadError() );
    ASSERT_EQ( false, events.hasWriteError() );
  }
}

//
// Overflow the write buffer and make sure the error flag is set
//
TEST( pipe_should, error_on_write_fail )
{
  IPinEvents<4> events;
  
  // Size 4 pipe can only hold 3 events, so feed it four events
  const std::vector<IPinEvent> golden = {
    { HW::PinState::INPUT_HIGH, Time::DeviceTimeUS{1} },
    { HW::PinState::INPUT_HIGH, Time::DeviceTimeUS{2} },
    { HW::PinState::INPUT_LOW,  Time::DeviceTimeUS{3} },
    { HW::PinState::INPUT_LOW,  Time::DeviceTimeUS{4} }
  };
  for ( const auto& event : golden ) {
    events.write( event );
  }

  // Should have error
  ASSERT_EQ( true , events.hasWriteError() );

  // First three events should be there - forth was lost.
  for( std::size_t i = 0; i < 3; ++i ) {
      ASSERT_EQ( golden[i], events.read());  
  }

  // Finishes with no events, but still has an error flag.
  ASSERT_EQ( false, events.hasEvents() );
  ASSERT_EQ( true , events.hasWriteError() );
}

//
// Make sure the read flag is set if we try to read when there's no data
//
TEST( pipe_should, error_on_read_fail )
{
  IPinEvent event = 
    { HW::PinState::INPUT_LOW,  Time::DeviceTimeUS{1} };

  // Go through some test permutations...
  //
  // prefeedCount:  How many events to add and remove at the beginning of the test
  for ( std::size_t prefeedCount = 0; prefeedCount < 100; ++prefeedCount )
  {
    IPinEvents<15> events;

    // Prefeed events
    for ( std::size_t prefeed = 0; prefeed < prefeedCount; ++prefeed ) {
      events.write( event );
      ASSERT_EQ( event, events.read() );
    }
 
    // Should be empty and error free
    ASSERT_EQ( false, events.hasEvents());
    ASSERT_EQ( false, events.hasReadError());
    ASSERT_EQ( false, events.hasWriteError());

    // Do the illegal read.  rval is garbage.
    events.read();

    // should now have the read error flag set.
    ASSERT_EQ( false, events.hasEvents());
    ASSERT_EQ( true,  events.hasReadError());
    ASSERT_EQ( false, events.hasWriteError());

  }
}


} // end Util namespace

