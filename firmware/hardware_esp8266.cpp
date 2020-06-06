#include <ESP8266WiFi.h>
#include "hardware_esp8266.h"

const std::unordered_map<HWI::Pin, int, EnumHash > HardwareESP8266::pinMap = {
};

const std::unordered_map<HWI::PinState, int, EnumHash > HardwareESP8266::pinStateMap = {
  //{ PinState::HOME_INACTIVE,  HIGH },    // Active low
  //{ PinState::HOME_ACTIVE,    LOW},      // Active low
};

void HardwareESP8266::DigitalWrite( Pin pin, PinState state )
{
  int actualPin = pinMap.at( pin );
  int actualState = pinStateMap.at( state );
  digitalWrite( actualPin, actualState );
}

void HardwareESP8266::PinMode( Pin pin, PinIOMode mode )
{
  int actualPin = pinMap.at( pin );
  pinMode( actualPin, mode == PinIOMode::M_OUTPUT ? OUTPUT : INPUT );
}

HWI::PinState HardwareESP8266::DigitalRead( Pin pin)
{
  int actualPin = pinMap.at( pin );
  return PinState::DUMMY_INACTIVE;
}

unsigned int HardwareESP8266::AnalogRead( Pin pin)
{
  return analogRead( A0 );
}


