#ifndef __UTIL_IPINEVENTS_H__
#define __UTIL_IPINEVENTS_H__

#include <array>
#include "hardware_types.h"       // For HW::PinState
#include "time_types.h"           // For Time::DeviceTimeUS

namespace Util {

/// A single event for IPinEvents
using IPinEvent = std::pair< HW::PinState, Time::DeviceTimeUS >; 

///
/// @brief Record changes in an input, and the time the change occured
///
/// This class records changes that occur on a single GPIO pin, and the time
/// the change occured at.
///
/// - Interrupt occurs because a GPIO pin changes state
///   - An interrupt handler is called
///   - The interrupt handler gets the time
///   - The interrupt handler gets the current state of the pin
///   - The interrupt handler records that information in IPinEvents  
///   - The interrupt handler exits
///
/// Use Example:  Encoder
/// 
/// - The Encoder Command's execute() method is called
///   - execute() looks at the IpinEvents for the two pins the encoder owns
///   - execute() computes the current encoder state using the pin changes
///   - execute() figures out the speed the encoder is moving from event times
///
/// The last job, "execute() figures out the speed the encoder is moving from
/// from event times", is the reason we need the extra complexity of interrupt
/// driven IO.  With polling we know that a pin state occured, but not 
/// precisely when it occured.
///
/// Here is background reading on using interrupts:  https://randomnerdtutorials.com/interrupts-timers-esp8266-arduino-ide-nodemcu/
///
/// The guidance on that tutorial says that the interrupt should be quick and
/// update a global variable.  That's more or less the approach that we'll
/// take.  An IpinEvents will be a true singleton, in that there will only be
/// a single instance for each class definition. 
///
/// @input[in] N   - the size of the event pipe
///
template< std::size_t N > 
class IPinEvents
{
  public:

  /// @brief Constructor
  IPinEvents() :
    writeSlot{ 0 },
    readSlot{ 0 },
    writeErrorFlag{ false },
    readErrorFlag{ false }
  {
  }

  /// @brief Write an event to the pipe.  Set error flag on overflow
  /// 
  void write( const IPinEvent& event ) noexcept {
    // Find the next empty write slot
    const std::size_t nextWriteSlot = ( writeSlot + 1 ) % N;

    //
    // An overflow will occur if advancing the writeSlot will move us to
    // the readSlot - we're wrapping around in that case.  If that would
    // happen, record nothing (no room), flag an error, and return.
    //
    if ( readSlot == nextWriteSlot ) 
    {
      writeErrorFlag = true;
      return;
    }

    //
    // Advance to the next write slot and record the event.
    //
    writeSlot = nextWriteSlot;
    events[writeSlot] = event;
  }

  /// @brief Are there events available to read?
  /// 
  bool hasEvents() {
    // If last slot where data was written is the same as the last slot we
    // read data from.  There's no data in the pipe;
    const bool noDataInPipe = ( readSlot == writeSlot );
    return !noDataInPipe;
  }

  /// @brief Read an event to the pipe.  Set error flag if there are no events
  /// 
  IPinEvent read() {
    if ( !hasEvents() ) {
      // no events - never should have been called :(
      readErrorFlag = true;
      return IPinEvent();   // return an empty event - it's garbage anyway
    }
    readSlot = ( readSlot + 1 ) % N;
    return events[ readSlot ];
  }

  /// @brief Did a read underflow error occur?
  bool hasReadError()  const { return readErrorFlag;  }
  /// @brief Did a write overflow error occur?
  bool hasWriteError() const { return writeErrorFlag; }

  private:
  
  // Index of the last slot where data was written
  size_t writeSlot;
  // Index of the last slot where data was read from
  size_t readSlot;
  // Set to true if we overflow on write
  bool writeErrorFlag;
  // Set to true if we underflow on read
  bool readErrorFlag;
  // The actual events.
  std::array< IPinEvent, N > events;
};

///
/// @brief A Filter that mostly conforms to an IPinEvents interface, and debounces
/// 
template< class DownstreamIpinEvents >
class IPinDebouncer
{
  public:

  ///
  /// @brief Constructor
  ///
  /// @brief param[in] downstreamArg - the downstream source of IPin Events
  /// @brief param[in] debouncTimeWindowArg 
  ///     Ignore events if a second event comes in less than 
  ///     "debouncTimeWidnowArg" microseconds after the first.
  /// 
  IPinDebouncer( DownstreamIpinEvents& downstreamArg, int deBounceTimeWindowArg  ) :
    downstream{ downstreamArg },
    deBounceTimeWindow{ deBounceTimeWindowArg }
  {
  }
  IPinDebouncer() = delete;

  ///
  /// @brief Are there any events left after debounce filtering? 
  /// 
  bool hasEvents() {
    drawEvents(); 
    return inPipe != 0;  
  }

  ///
  /// @brief Read a debounced event
  /// 
  IPinEvent read() {
    // Fill the mini pipe
    drawEvents();

    // Crash if empty - this shouldn't have been called.
    if ( inPipe == 0 ) {
      for( ;; );  
    }

    // Non-empty.  Get the first entry in the pipe to return.
    IPinEvent rval = pipe[0];

    // Shift everthing over one @ return.
    pipe[0] = pipe[1];
    --inPipe;
    return rval;
  }

  private:

  //
  // Draw events from the downstream source, squashing any that are too
  // close together in time.
  // 
  void drawEvents() 
  {
    for ( ;; ) {
      // Fill the pipe to max
      while ( inPipe < 2 && downstream.hasEvents() ) {
        pipe[ inPipe ] = downstream.read();
        ++inPipe;
      }

      // If the pipe is less than 2, we're out of events downstream.
      // Just return, there's nothing more to debounce.
      if ( inPipe < 2 ) { return; }

      // If the two times are far enough apart then return.  We can
      // feed pipe[0] the next time that read is called.
      if ( pipe[ 1 ].second - pipe[ 0 ].second > deBounceTimeWindow )
      {
        return;
      }

      // Squash pipe[0] and then loop around to try re-reading pipe[1],
      // and maybe doing further squashes.
      pipe[0] = pipe[1];
      --inPipe;
    }
  }

  private:

  DownstreamIpinEvents& downstream;
  const int deBounceTimeWindow;

  size_t inPipe = 0;
  IPinEvent pipe[2];
};

using MergedEvent = std::pair< int, IPinEvent >;

///
/// @brief Merge two IPin Events streams in chronological order, without doing
///        any dynamic allocation.
///
/// Written for the encoder, which has two pins that need to be sorted chronologically
/// 
template< class IPinEventsA, class IPinEventsB >
class IPinEventMerger
{
  public: 

  IPinEventMerger( IPinEventsA& eventsAArg, IPinEventsB& eventsBArg )
  : eventsA{ eventsAArg },
    eventsB{ eventsBArg }
  {
  }

  // @brief Are there any events available for read?
  bool hasEvents()
  {
    drawEvents();
    return hasEventA || hasEventB;
  }

  /// @brief Read the next event in the merged stream
  MergedEvent read()
  {
    drawEvents();
    if ( !hasEventA && !hasEventB ) 
    {
      // No events - programmer logic failure.
      // Just lock it and force the WTD to crash
      for( ;; );
    }
    if (  hasEventA && !hasEventB ) 
    {
      // Only an event A
      hasEventA = false;
      return MergedEvent( 0, eventA ); 
    }
    if ( !hasEventA &&  hasEventB )
    {
      // Only an event B
      hasEventB = false;
      return MergedEvent( 1, eventB ); 
    }
    // Both A and B.  Choose the lowest time
    if ( eventB.second < eventA.second ) 
    {
      hasEventB = false;  
      return MergedEvent( 1, eventB ); 
    }
    // Ties go to A 
    hasEventA = false;  
    return MergedEvent( 0, eventA ); 
  }

  private:

  //
  // Load events up from the downstream interfaces, if available.
  //
  void drawEvents() 
  {
    if ( !hasEventA && eventsA.hasEvents() ) {
      eventA = eventsA.read();
      hasEventA = true;
    }  
    if ( !hasEventB && eventsB.hasEvents() ) {
      eventB = eventsB.read();
      hasEventB = true;
    }  
  }

  bool hasEventA = false;
  bool hasEventB = false;
  IPinEvent eventA;
  IPinEvent eventB;

  IPinEventsA& eventsA;
  IPinEventsB& eventsB;
};

using GreyCodeTime = std::pair< unsigned int, Time::DeviceTimeUS >;

///
/// Update an existing grey code given two existing Pin event streams
///
/// - Takes two existing event steams, a greycode, and a debounce time window as input.
/// - Debounces the two event streams
/// - Merges the debounced event streams in chronological order
/// - Updates the existing greycode using the merged stream
/// - Outputs, via the read() method, the new greycodes and the time they transtioned
/// 
/// @param[in] IPinEvents0 - The type of the Pin 0 events steam
/// @param[in] IPinEvents1 - The type of the Pin 1 events steam
/// 
template< class IPinEvents0, class IPinEvents1 > 
class GreyCodeTracker
{
  public:

  // Type of the Pin 0 Debounced stream
  using DeBounced0  =  IPinDebouncer<IPinEvents0>;
  // Type of the Pin 1 Debounced stream
  using DeBounced1  =  IPinDebouncer<IPinEvents1>;
  // Type of the Merged events stream
  using MergedIPins =  IPinEventMerger<DeBounced0, DeBounced1>;

  ///
  /// @brief Constructor
  ///
  /// @param[in]    startGreyCode   The initial greycode
  /// @param[in]    eventsPin0Arg   The stream of events that occur on Pin 0
  /// @param[in]    eventsPin1Arg   The stream of events that occur on Pin 1
  /// @param[in]    deBounceTimeWindow  Argument for the event debouncer class
  /// 
  GreyCodeTracker( 
    unsigned      startGreyCode, 
    IPinEvents0&  eventsPin0Arg,
    IPinEvents0&  eventsPin1Arg,
    int deBounceTimeWindow
  ) :
    currentGreyCode{ startGreyCode },
    eventsPin0{ eventsPin0Arg },
    eventsPin1{ eventsPin1Arg },
    deBounced0{ eventsPin0, deBounceTimeWindow }, 
    deBounced1{ eventsPin1, deBounceTimeWindow }, 
    mergedIPins{ deBounced0, deBounced1 }
  {
  }
  
  GreyCodeTracker() = delete;

  ///
  /// @brief Are there more greycode events to be processed?
  ///
  bool hasEvents()
  {
    return mergedIPins.hasEvents();
  }


  ///
  /// @brief Get the next Greycode transition & the time it occured at
  ///
  GreyCodeTime read()
  {
    if ( !hasEvents()) {
      for ( ;; );   // Programmer Error!  TODO - create proper "assert" system
    }

    // Get & Unpack the Event
    //
    const MergedEvent           event     { mergedIPins.read() };
    const int                   pin       { event.first }; 
    const HW::PinState          pinState  { event.second.first }; 
    const Time::DeviceTimeUS    time      { event.second.second }; 

    // Use the event to update the Grey Code
    //
    if ( pin == 0 ) {
      if ( pinState == HW::PinState::INPUT_HIGH ) 
      {
        // Make sure Bit 0 is turned on
        currentGreyCode |= 1;
      }
      else {
        // Make sure Bit 0 is turned off, but keep bit 1.
        currentGreyCode &= 2;
      }
    }
    else {
      if ( pinState == HW::PinState::INPUT_HIGH ) 
      {
        // Make sure Bit 1 is turned on
        currentGreyCode |= 2;
      }
      else {
        // Make sure Bit 1 is turned off, but keep bit 0.
        currentGreyCode &= 1;
      }
    }

    // Return Greycode & Timestamp
    return GreyCodeTime{ currentGreyCode, time };  
  }

  private:

  unsigned int  currentGreyCode;
  IPinEvents0&  eventsPin0;
  IPinEvents1&  eventsPin1;
  DeBounced0    deBounced0;
  DeBounced1    deBounced1;
  MergedIPins   mergedIPins;
};

} // End util namespace


#endif

