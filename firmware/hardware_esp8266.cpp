#include <ESP8266WiFi.h>
#include "hardware_esp8266.h"
#include <memory>

namespace
{
const std::unordered_map<HW::Pin, int, EnumHash > pinMap = 
{
  { HW::Pin::MOTOR0_PIN0,     0     },
  { HW::Pin::MOTOR0_PIN1,     2     },
  { HW::Pin::MOTOR1_PIN0,     16    },
  { HW::Pin::MOTOR1_PIN1,     15    },
  { HW::Pin::ENCODER0_PIN0,   14    },
  { HW::Pin::ENCODER0_PIN1,   12    },
  { HW::Pin::ENCODER1_PIN0,   13    },
  { HW::Pin::ENCODER1_PIN1,   4     },
  { HW::Pin::SR04_TRIG,       1     },
  { HW::Pin::SR04_ECHO,       5     }
};

const std::vector< HW::Pin > interruptInputs = {
  HW::Pin::ENCODER0_PIN0,
  HW::Pin::ENCODER0_PIN1,
  HW::Pin::ENCODER1_PIN0,
  HW::Pin::ENCODER1_PIN1
};

const std::unordered_map<HW::PinState, int, EnumHash > pinStateMap = {
  { HW::PinState::MOTOR_POS,      HIGH  },
  { HW::PinState::MOTOR_NEG,      LOW   },
  { HW::PinState::ECHO_ON,        HIGH  },
  { HW::PinState::ECHO_OFF,       LOW   }
};

class InputInterruptHandler
{
  public:

  InputInterruptHandler( std::shared_ptr< Time::HST> hstArg, HW::Pin pin )
    : hst{ hstArg }
  {
    // Cache the actual pin
    esp8266Pin = pinMap.at( pin ); 
  }
  
  void ICACHE_RAM_ATTR interrupt()
  {
    // Record the pin state & time, and get out of here.
    events.write( Util::IPinEvent(
        digitalRead( esp8266Pin ) == HIGH ? 
            HW::PinState::INPUT_HIGH : 
            HW::PinState::INPUT_LOW,
        hst->usSinceDeviceStart())
    );
  }

  HW::IEvent& getEvents() {
    return events;
  }

  private:
  HW::IEvent events;
  std::shared_ptr< Time::HST > hst; 
  int esp8266Pin;
};

std::array<int, static_cast<size_t>(HW::Pin::END_OF_PINS)  > fastAbstractToRealPin;
std::array<int, static_cast<size_t>(HW::PinState::END_OF_PIN_STATES) > fastAbstractToRealPinState; 
std::array<std::unique_ptr<InputInterruptHandler>, static_cast<size_t>(HW::Pin::END_OF_PINS ) > pinToInputHandler;

//
// Programming with raw pointers and arrays, just like the 90s.
// The ESP8266 interrupt handler HAS to be in RAM.  Raw arrays and
// smart pointers aren't good C++ anymore, but in this case they're
// easy to reason about.
// 
InputInterruptHandler* pinToInputHandlerRaw[ static_cast<size_t>(HW::Pin::END_OF_PINS) ];

//
// TODO - enable interrupts and add for each encoder type
// 
//void ICACHE_RAM_ATTR leftEncoderPin0Int()
//{
//  InputInterruptHandler* handler = pinToInputHandlerRaw[ static_cast<size_t>(HW::Pin::ENCODER0_PIN0) ];
//  handler->interrupt();
//}

} // end anonymous namespace

namespace HW {
HardwareESP8266::HardwareESP8266( std::shared_ptr< Time::HST> hst )
{
  // create cache for pin id
  for ( size_t index = 0 ; index < fastAbstractToRealPin.size(); ++index ) 
  {
    fastAbstractToRealPin[ index ] = 0;
  }
  for ( const auto& entry :  pinMap ) 
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
  for ( const auto& entry : pinStateMap) 
  {
    size_t index = static_cast<size_t>(entry.first);
    if ( index >= fastAbstractToRealPinState.size() ) {
      for (;; );    // think of it like an assert
    }
    fastAbstractToRealPinState[ index ] = entry.second;
  }

  pinMode( 1, FUNCTION_3 );
  pinMode( 3, FUNCTION_3 );

  for ( size_t index = 0; index < static_cast<size_t>(HW::Pin::END_OF_PINS); ++index ) {
    pinToInputHandlerRaw[ index ] = nullptr;
  }
  for( Pin interruptInput : interruptInputs ) 
  {
    size_t slot = static_cast<size_t>(interruptInput);
    pinToInputHandler[ slot ] = std::unique_ptr<InputInterruptHandler>( new InputInterruptHandler(hst, interruptInput ));
    pinToInputHandlerRaw[ slot ] = pinToInputHandler[ slot ].get();
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

HW::PinState HardwareESP8266::DigitalRead( Pin pin)
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

IEvent& HardwareESP8266::GetInputEvents( Pin pin )
{
  auto& handler = pinToInputHandler[ static_cast<size_t>( pin ) ];

  // Fake an interrupt for now
  InputInterruptHandler* rawHandler = pinToInputHandlerRaw[ static_cast<size_t>(pin ) ];
  rawHandler->interrupt();

  return handler->getEvents();
}
}

