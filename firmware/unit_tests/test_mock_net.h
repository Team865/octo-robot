///
/// @brief Testing Mock for network events
/// 

#ifndef __TEST_MOCK_NET_H__
#define __TEST_MOCK_NET_H__

#include <algorithm>
#include "../firmware_v1/net_interface.h"
#include "test_mock_event.h"
#include "test_mock_hardware.h"

/// @brief Dummy mock for net connectins
///
/// Fill in if needed for unit tests.
///
class NetMockSimpleConnection: public NetConnection
{
  public:

  bool getString( std::string& string ) {
    string = "";
    return true;
  }
  operator bool(void ) {
    return true;
  }
  void reset(void ) 
  {
  }

  void writePushImpl( NetPipe&) override {
    // should never happen
    assert(0);
  }


  Time::TimeUS execute() override {
    return Time::TimeUS( 5 * Time::USPerS );
  }

}; 
///
/// @brief Testing Mock for network events
/// 
/// NetMockSimpleTimed implements a mock Network Interface (NetInterface) 
/// that's used in unit testing.  The class implements testing mocks for 
/// the interfaces required by the NetInterface class.  In additiona to
/// that, the class does the following:
///
/// - Maintain Time.  
///     The class simulates the passage of time.  advanceTime is called to 
///     "move" time forward
/// - Record Output.  
///     Whenever a caller outputs a string to the network interface,
///     the string and the time the string was outputted are recorded
///     as an event.  Tests can use this log to verify that the data
///     being sent to the caller matches a golden result.
/// - Simulate Input.
///     On class construction, the caller can specify a series of input
///     events and the time those events occur at.  i.e.,  the caller can
///     say 'at time 20ms the string "HOME" will arrive from the network'
/// 
class NetMockSimpleTimed: public NetInterface
{
  public:

  struct category: public beefocus_tag {};

  ///
  /// @brief Create a network mock given a vector of Input Events
  ///
  /// @param[in] inputEventsArg - Simulated input from the network
  ///   interface.  A vector of input strings and the time those
  ///   strings arrive at.
  ///
  NetMockSimpleTimed( const TimedStringEvents& inputEventsArg)
    : inputEvents{inputEventsArg}, 
      time{0},
      nextInputEvent{inputEvents.begin()},
      currentOutput{}
  {
  }
  
  ///
  /// @brief Create a simple network with 1 input event at time=0
  ///
  /// @param[in] string - A C style string with the single input
  ///
  NetMockSimpleTimed( const char* string )
    : inputEvents{ TimedStringEvents( {{ 0, std::string( string ) }}) },
      time{0},
      nextInputEvent{inputEvents.begin()},
      currentOutput{}
  {
  }

  ///
  /// @brief Create a simple network with no input events.
  ///
  NetMockSimpleTimed()
    : inputEvents{},
      time{0},
      nextInputEvent{inputEvents.begin()},
      currentOutput{}
  {
  }

  // delete unused operators for safety
  NetMockSimpleTimed( const HW::MockTimed& ) = delete;
  NetMockSimpleTimed& operator=( const NetMockSimpleTimed& ) = delete;


#ifdef GONE
  ///
  /// @brief Get input from the net interface
  /// @param[out] returnString - A reference to the string that's populated
  ///                            if there's any input
  /// @return     true if there's input, false otherwise.
  ///
  bool getString( std::string& returnString ) override
  {
    // Set the string to a default.
    returnString = "";

    if ( nextInputEvent == inputEvents.end() )
      return false;   // No more events to process
    if ( nextInputEvent->time > time )
      return false;   // Still events, but they haven't occurred yet

    // Got one.
    returnString = nextInputEvent->event;   // Populate Return String
    nextInputEvent++;                       // Go to the next input event.
    return true;
  }

  /// 
  /// @brief Output a chacacter to the interface usign a streaming operator
  ///
  /// @param[in]  c   The character to output
  /// @return         The Interface
  ///
  /// Note:  Other operators (i.e.  NetInterface& operator<<( string ))
  ///        are declared using templates in simple_ostream.h 
  /// 
  /// Works by appending by character to the last string output event
  /// in the outputEvents vector.  
  /// 
  /// Algorithm:
  ///
  /// 1.  If c != 'n', append it to the currentOutput string.
  /// 2.  If c == 'n', append the currentOutput to the outputEvents.
  ///
  void onechar(char c ) 
  {
    if ( c != '\n' )
    {
      // 1.  If c != 'n', append it to the currentOutput string.
      currentOutput  += c;
    }
    else
    {
      // 2.  If c == 'n', append the currentOutput to the outputEvents.
      outputEvents.emplace_back(TimedStringEvent(time, currentOutput ));
      currentOutput = "";
    }
  }
#endif

  const char* debugName() override { return "NetMockSimpleTimed"; }

#ifdef GONE
  std::streamsize write( const char_type* s, std::streamsize n) override
  {
    for ( std::streamsize i = 0; i < n; ++i )
    {
      onechar( s[i] );
    }
    return n;
  }

  void flush() override
  {
  }
#endif

  ///
  /// @brief      Advance network mock time by "ticks" ms
  /// @param[in]  The amount of time by, in ms
  ///
  void advanceTime( int ticks )
  {
    time+=ticks;
  }

  ///
  /// @brief  Get output events for golden result comparison
  ///
  /// @return All the output that was recorded on the network interface
  ///
  /// Example:
  ///
  /// @code
  ///   netMockSimpleTimed net;   // No input
  ///
  ///   net.advanceTime( 10 );
  ///   net << "This is a test\n";
  ///   net.advanceTime( 15 );
  ///   net << "More test data\n";
  ///
  ///   TimedStringEvents goldenNet = {
  ///     { 10,   "This is a test"    },
  ///     { 15,   "More test data"    }
  ///   }
  ///
  ///   ASSERT_EQ( goldenNet, net.getOutput() );
  /// @endcode
  /// 
  const TimedStringEvents& getOutput() 
  {
    return outputEvents;
  }

  NetConnection& get() {
    return defaultConnection;
  }

  private:
  /// @brief  Input events to be sent back to the caller
  const TimedStringEvents inputEvents;
  /// @brief  Current Time
  int time;
  /// @brief  The next input event that needs to be processed.
  TimedStringEvents::const_iterator nextInputEvent;
  /// @brief  The current string that's being written to 
  std::string currentOutput;
  /// @brief  Recorded output events
  TimedStringEvents outputEvents;
  NetMockSimpleConnection defaultConnection;
};

///
/// @brief Helper function to filter out comments
/// 
/// @param[in]  inEvents - Raw string event list.
/// @return     A list of string events with comments removed.
/// 
inline TimedStringEvents testFilterComments( 
  const TimedStringEvents& inEvents ) 
{
  TimedStringEvents outEvents;

  std::copy_if( inEvents.begin(), inEvents.end(), 
    std::back_inserter( outEvents ), [] ( const TimedStringEvent& e ) 
    {
      return e.event.size() == 0 ? true  :    // "" is     not a comment
             e.event[0] == '#'   ? false :    // "# foo"   is a comment
                                   true;      // default:  not a comment
    } 
  );

  return outEvents;
} 

#endif

