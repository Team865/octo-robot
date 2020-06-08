#include "action_motor.h"
#include "wifi_debug_ostream.h"

namespace Command{

Motor::Motor( std::shared_ptr<HWI> hwiArg, std::shared_ptr<DebugInterface> debugArg, std::shared_ptr<NetInterface> netArg, HWI::Pin pin0Arg,  HWI::Pin pin1Arg ) :
      hwi { hwiArg },
      debug { debugArg },
      net { netArg },
      pin0{ pin0Arg },
      pin1{ pin1Arg},
      dir { Motor::Dir::FORWARD },
      speed{ 0 },
      counter{ 0 },
      lastPulse{ Pulse::NONE }
{
  hwi->PinMode(pin0,  HWI::PinIOMode::M_OUTPUT );
  hwi->PinMode(pin1,  HWI::PinIOMode::M_OUTPUT );
  doPulse( Pulse::NONE );
}

void Motor::doPulse( Motor::Pulse pulse )
{
  switch( pulse ) 
  {
    case Pulse::FORWARD:
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_POS );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_NEG );
      break;
    case Pulse::BACKWARD:
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_NEG );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_POS );
      break;
    case Pulse::NONE:
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_NEG );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_NEG );
      break;
  }
  lastPulse = pulse;
}

void Motor::doPulseIfChanged( Motor::Pulse pulse )
{
  if ( lastPulse != pulse )
  {
    doPulse( pulse );
  }
}

Time::TimeUS Motor::execute() 
{
  counter = counter + 1;

  const bool pulse_high = counter & 1;
  Time::TimeUS current_pulse = pulse_high ? 
      Time::TimeUS( speed * 500 ) : 
      Time::TimeUS( ( 100-speed ) * 500 );

  if ( current_pulse == Time::TimeUS( 0 ) ) {
    return current_pulse;
  }

  if ( !pulse_high ) {
    doPulseIfChanged( Pulse::NONE );
  }
  else {
    if ( dir == Motor::Dir::FORWARD ) {
      doPulseIfChanged( Pulse::FORWARD );
    }
    else {
      doPulseIfChanged( Pulse::BACKWARD );
    }
  }

  return current_pulse;
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

