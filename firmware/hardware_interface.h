#ifndef __HARDWARE_INTERFACE_H__
#define __HARDWARE_INTERFACE_H__

#include <unordered_map>
#include <string>
#include "basic_types.h"
#include "hardware_types.h"   // pin enums & related functions

namespace HW {

/// @brief Interface to the hardware
class I
{
  public:

  virtual ~I() {}

  virtual void DigitalWrite( Pin pin, PinState state ) = 0;
  virtual void PinMode( Pin pin, PinIOMode mode ) = 0;
  virtual unsigned AnalogRead( Pin pin ) = 0;
  virtual PinState DigitalRead( Pin pin) = 0;
};
};  // End HW namespace

#endif

