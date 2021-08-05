#ifndef __HARDWARE_ARDUINO_H__
#define __HARDWARE_ARDUINO_H__

#include "hardware_interface.h"
#include "time_hst.h"
#include "Wire.h"
#include <cstddef>

namespace HW {
class HardwareESP8266: public I
{
  public:

  HardwareESP8266( std::shared_ptr< Time::HST> hstArg );
  virtual ~HardwareESP8266() {}

  void      DigitalWrite( Pin pin, PinState state ) override;
  void      PinMode( Pin pin, PinIOMode state ) override;
  PinState  DigitalRead( Pin pin) override;
  unsigned  AnalogRead( Pin pin) override;
  IEvent&   GetInputEvents( Pin pin) override;
  void      LEDSet( unsigned int led, unsigned char r, unsigned char g, unsigned char b ) override;
  void      LEDUpdate() override;
  void      beginTransmission( int address ) override;
  void      endTransmission() override;
  void      write( std::basic_string_view< std::byte> bytes ) override;
  void      WireBeginTransmission(int address) override {Wire.beginTransmission(address);};
  bool      WireEndTransmission() override {return((Wire.endTransmission() == 0) ? true : false);};
  void      WireWrite(int data) override {Wire.write(data);};
  int       WireRead() override {return(Wire.read());};
  int       WireAvailable() override {return(Wire.available());};
  int       WireRequestFrom(int address, int quantity) override {return(Wire.requestFrom(address, quantity));};
};
};

#endif
