#include <ESP8266WiFi.h>
#include "hardware_esp8266.h"

const std::unordered_map<HWI::Pin, int, EnumHash > HardwareESP8266::pinMap = 
{
  { Pin::MOTOR0_PIN0,     14    },
  { Pin::MOTOR0_PIN1,     4     },
  { Pin::MOTOR1_PIN0,     0     },
  { Pin::MOTOR1_PIN1,     2     },
  { Pin::ENCODER0_PIN0,   12    },
  { Pin::ENCODER0_PIN1,   13    },
  { Pin::ENCODER1_PIN0,   16    },
  { Pin::ENCODER1_PIN1,   5     }
};

const std::unordered_map<HWI::PinState, int, EnumHash > HardwareESP8266::pinStateMap = {
  { PinState::MOTOR_POS,      HIGH  },
  { PinState::MOTOR_NEG,      LOW   },
};

namespace
{
  std::array<int, static_cast<size_t>(HWI::Pin::END_OF_PINS)  > fastAbstractToRealPin;
  std::array<int, static_cast<size_t>(HWI::PinState::END_OF_PIN_STATES) > fastAbstractToRealPinState; 
} // end anonymous namespace

HardwareESP8266::HardwareESP8266()
{
  // create cache for pin id
  for ( size_t index = 0 ; index < fastAbstractToRealPin.size(); ++index ) 
  {
    fastAbstractToRealPin[ index ] = 0;
  }
  for ( const auto& entry :  HardwareESP8266::pinMap ) 
  {
    size_t index = static_cast<size_t>(entry.first);
    if ( index >= fastAbstractToRealPin.size() ) {
      for (;; );    // think of it like an assert
    }
    fastAbstractToRealPin[ index ] = entry.second;
  }
  // create cache for pin state
  for ( size_t index = 0 ; index < fastAbstractToRealPinState.size(); ++index ) 
  {
    fastAbstractToRealPinState[ index ] = 0;
  }
  for ( const auto& entry :  HardwareESP8266::pinStateMap) 
  {
    size_t index = static_cast<size_t>(entry.first);
    if ( index >= fastAbstractToRealPinState.size() ) {
      for (;; );    // think of it like an assert
    }
    fastAbstractToRealPinState[ index ] = entry.second;
  }
}

void HardwareESP8266::DigitalWrite( Pin pin, PinState state )
{
  digitalWrite( 
      fastAbstractToRealPin     [ static_cast<size_t> (pin)   ],
      fastAbstractToRealPinState[ static_cast<size_t> (state) ] );
}

void HardwareESP8266::PinMode( Pin pin, PinIOMode mode )
{
  int actualPin = pinMap.at( pin );
  pinMode( actualPin, mode == PinIOMode::M_OUTPUT ? OUTPUT : INPUT );
}

HWI::PinState HardwareESP8266::DigitalRead( Pin pin)
{
  const int actualPin = fastAbstractToRealPin[ static_cast<size_t>( pin ) ];
  const int actualState = digitalRead( actualPin );
  return actualState == HIGH ? PinState::INPUT_HIGH : PinState::INPUT_LOW;
}

unsigned int HardwareESP8266::AnalogRead( Pin pin)
{
  (void) pin;
  return analogRead( A0 );
}


