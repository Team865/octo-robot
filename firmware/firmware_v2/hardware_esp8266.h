#ifndef __HARDWARE_ARDUINO_H__
#define __HARDWARE_ARDUINO_H__

#include "hardware_interface.h"
#include "time_hst.h"
#include "Wire.h"
#include <SoftWire.h>
#include <AsyncDelay.h>
#include <cstddef>

namespace HW {
class HardwareESP8266: public I
{
  private:
  SoftWire sw = SoftWire(13, 12);

  char swTxBuffer[16];
  char swRxBuffer[16];
  
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
  
  void      WireBeginTransmission( int i2c_bus, int address ) override {
    if(i2c_bus == 0){
      Wire.beginTransmission(address);
    }
    else{
      sw.beginTransmission(address);
    };
  };
  bool      WireEndTransmission( int i2c_bus ) override {
    if(i2c_bus == 0){
      return((Wire.endTransmission() == 0) ? true : false);
    }
    else{
      return((sw.endTransmission() == 0) ? true : false);
    };
  };
  void      WireWrite( int i2c_bus, int data) override {
    if(i2c_bus == 0){
      Wire.write(data);
    }
    else{
      sw.write(data);
    }
  };
  int       WireRead( int i2c_bus ) override {
    if(i2c_bus == 0){
      return(Wire.read());
    }
    else{
      return(sw.read());
    };
  };
  int       WireAvailable(  int i2c_bus ) override {
    if(i2c_bus == 0){
      return(Wire.available());
    }
    else{
      return(sw.available());
    };
  };
  int       WireRequestFrom( int i2c_bus, int address, int quantity) override {
    if(i2c_bus == 0){
      return(Wire.requestFrom(address, quantity));
    }
    else{
      return(sw.requestFrom(address, quantity));
    }
  };
    
};
};

#endif
