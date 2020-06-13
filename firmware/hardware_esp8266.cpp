#include <ESP8266WiFi.h>
#include "hardware_esp8266.h"

const std::unordered_map<HWI::Pin, int, EnumHash > HardwareESP8266::pinMap = 
{
  { Pin::MOTOR0_PIN0,     14    },
  { Pin::MOTOR0_PIN1,     4     },
  { Pin::ENCODER0_PIN0,   12    },
  { Pin::ENCODER0_PIN1,   13    }
};

const std::unordered_map<HWI::PinState, int, EnumHash > HardwareESP8266::pinStateMap = {
  { PinState::MOTOR_POS,      HIGH  },
  { PinState::MOTOR_NEG,      LOW   },
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
  int actualState = digitalRead( actualPin );
  return actualState == HIGH ? PinState::INPUT_HIGH : PinState::INPUT_LOW;
}

unsigned int HardwareESP8266::AnalogRead( Pin pin)
{
  (void) pin;
  return analogRead( A0 );
}


