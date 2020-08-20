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


} // End util namespace

#ifdef TODO_SAMPLE_HANDLER

  IpinEvents<16> pinEvents; 

  void interrupt() {
    pinEvents.write(IPinEvent( hwi->DigitalRead(pin) , hst->usSinceDeviceStart()));   
  }
#endif

#endif

