#include "action_motor.h"
#include "wifi_debug_ostream.h"

namespace Action {

Motor::Motor( std::shared_ptr<HWI> hwiArg, std::shared_ptr<DebugInterface> debugArg, std::shared_ptr<NetInterface> netArg, HWI::Pin pin0Arg,  HWI::Pin pin1Arg ) :
      hwi { hwiArg },
      debug { debugArg },
      net { netArg },
      pin0{ pin0Arg },
      pin1{ pin1Arg},
      dir { Motor::Dir::FORWARD },
      speed{ 0 },
      counter{ 0 }
{
  hwi->PinMode(pin0,  HWI::PinIOMode::M_OUTPUT );
  hwi->PinMode(pin1,  HWI::PinIOMode::M_OUTPUT );
}

unsigned int Motor::loop() 
{
  counter = counter + 1;

  const bool pulse_high = counter & 1;
  const int current_pulse = pulse_high ? speed : 100-speed;

  if ( current_pulse == 0 ) {
    return current_pulse;
  }

  if ( !pulse_high ) {
    hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_NEG );
    hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_NEG );
  }
  else {
    if ( dir == Motor::Dir::FORWARD ) {
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_POS );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_NEG );
    }
    else {
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_NEG );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_POS );
    }
  }

  return current_pulse * 500;
}

void Motor::setSpeed( int percent )
{
  dir = percent >= 0 ? Dir::FORWARD : Dir::BACKWARDS;
  speed = percent >= 0 ? percent : -percent;
}


const char* Motor::debugName()
{
  return "Motor";
}

} // End Action Namespace

