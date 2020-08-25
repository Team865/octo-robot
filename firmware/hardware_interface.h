#ifndef __HARDWARE_INTERFACE_H__
#define __HARDWARE_INTERFACE_H__

#include <unordered_map>
#include <string>
#include "basic_types.h"
#include "hardware_types.h"     // pin enums & related functions
#include "util_ipinevents.h"    // for the IPinEvent interface 

namespace HW {

//
// Start with a 64 event buffer.  That should be overkill unless the encoder
// bounces and generates a pile of events.  
//
// If that does seem to happen, maybe the interrupt routine can detect the 
// fact that the pin is changing quickly and just over-write the top entry
// in the pipe.  I'd prefer to not have to deal with that inside the interrupt
// route if possible.
//
using IEvent = Util::IPinEvents<64>;

/// @brief Interface to the hardware
class I
{
  public:

  virtual ~I() {}

  virtual void DigitalWrite( Pin pin, PinState state ) = 0;
  virtual void PinMode( Pin pin, PinIOMode mode ) = 0;
  virtual unsigned AnalogRead( Pin pin ) = 0;
  virtual PinState DigitalRead( Pin pin) = 0;
  virtual IEvent& GetInputEvents( Pin pin ) = 0;
  virtual void LEDSet( unsigned int led, unsigned char r, unsigned char g, unsigned char b ) = 0;
  virtual void LEDUpdate() = 0;
};
};  // End HW namespace

#endif

