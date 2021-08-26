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
  virtual void WireBeginTransmission( int i2c_bus, int address ) = 0;
  virtual void WireWrite( int i2c_bus, int data ) = 0;
  virtual bool WireEndTransmission( int i2c_bus ) = 0;
  virtual int WireRequestFrom( int i2c_bus, int address, int quantity) = 0;
  virtual int WireAvailable( int i2c_bus ) = 0;
  virtual int WireRead( int i2c_bus ) = 0;
  inline int WireRead2( int i2c_bus ) {
    const int upper = WireRead(i2c_bus);
    const int lower = WireRead(i2c_bus);
    return ( ( upper << 8) | lower ); 
  }
};
};  // End HW namespace

#endif

