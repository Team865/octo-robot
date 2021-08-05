#ifndef __HARDWARE_INTERFACE_H__
#define __HARDWARE_INTERFACE_H__

#include <unordered_map>
#include <string>
#include <string_view>
#include <cstddef>
#include "basic_types.h"
#include "hardware_types.h"     // pin enums & related functions
#include "util_ipinevents.h"    // for the IPinEvent interface 

namespace HW {
//
// 128 event buffer.  I've noticed the encounter input bouncing with a 
// 10us delay between bounces.  Missed events == unreliable encoder.
//
using IEvent = Util::IPinEvents<128,200>;

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
  virtual void beginTransmission( int address ) = 0;
  virtual void endTransmission() = 0;
  virtual void write( std::basic_string_view< std::byte > data ) = 0;
  virtual void WireBeginTransmission( int address ) = 0;
  virtual void WireWrite( int data ) = 0;
  virtual bool WireEndTransmission() = 0;
  virtual int WireRequestFrom(int address, int quantity) = 0;
  virtual int WireAvailable() = 0;
  virtual int WireRead() = 0;
};
};  // End HW namespace

#endif

