#include "command_motor.h"
#include "wifi_debug_ostream.h"

namespace Command{

Motor::Motor( 
  std::shared_ptr<HWI> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg, 
  HWI::Pin pin0Arg,  
  HWI::Pin pin1Arg 
) :
  hwi { hwiArg }, debug { debugArg }, net { netArg }, 
  pin0{ pin0Arg }, pin1{ pin1Arg},
  dir { Motor::Dir::FORWARD }, speedAsPercent{ 0 }, counter{ 0 }, 
  lastPulse{ Pulse::NONE }
{
  // Configure hardware pins for output
  hwi->PinMode(pin0,  HWI::PinIOMode::M_OUTPUT );
  hwi->PinMode(pin1,  HWI::PinIOMode::M_OUTPUT );
  // set output to "off" (both lines powered down)
  doPulse( Pulse::NONE );
}

//
// Set the motor controller output as per pulse
// 
void Motor::doPulse( Motor::Pulse pulse )
{
  switch( pulse ) 
  {
    case Pulse::FORWARD:
      // Forward, one input on, the other input off
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_POS );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_NEG );
      break;
    case Pulse::BACKWARD:
      // Backward, one input off, the other input on
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_NEG );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_POS );
      break;
    case Pulse::NONE:
      // Stopped,  Both inputs off.
      hwi->DigitalWrite( pin0, HWI::PinState::MOTOR_NEG );
      hwi->DigitalWrite( pin1, HWI::PinState::MOTOR_NEG );
      break;
  }
  lastPulse = pulse;
}

// Set the motor controller output, but only if it's changed.
void Motor::doPulseIfChanged( Motor::Pulse pulse )
{
  if ( lastPulse != pulse )
  {
    doPulse( pulse );
  }
}

//
// Standard execute method
//
// Controls the speed of the motor by pulsing power to the motor.  If the
// motor is 50% on then execute generates a pulse that's "on" 50% of the
// time.  If the motor is 75% on, the pulse is "on" 75% of the time.
//
// See https://socratic.org/questions/how-do-you-calculate-the-period-and-frequency for terms
// 
// 1. Figure out if we're turning the motor on or off.
// 2. Figure out the pulse duration.  This is a function of speed
// 3. Early exit if we're spending no time in the current state
// 4. Set the hardware to the new state
// 5. Tell the schedule to pause, as per the pulse's duration
// 
Time::TimeUS Motor::execute() 
{
  counter = counter + 1;

  // 1. Figure out if we're turning the motor on or off.
  //
  const bool shouldTurnMotorOn= ( counter & 1 ) == 1;

  // 2. Figure out the pulse duration.  This is a function of speed
  //
  const unsigned int percentOfPeriodForCurrentState= 
      shouldTurnMotorOn ? speedAsPercent : (100 - speedAsPercent );
  const Time::TimeUS pulseDuration= 
      Time::TimeUS( periodInMS * Time::USPerMs * percentOfPeriodForCurrentState / 100 ); 

  // 3. Early exit if we're spending no time in the current state
  // 
  if ( pulseDuration == Time::TimeUS( 0 ) ) {
    return pulseDuration;
  }

  // 4. Set the hardware to the new state
  // 
  if ( !shouldTurnMotorOn) {
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

  // 5. Tell the schedule to pause, as per the pulse's duration
  ///
  return pulseDuration;
}

//
// Set the current speed and direction
// 
void Motor::setSpeed( int percent )
{
  dir = percent >= 0 ? Dir::FORWARD : Dir::BACKWARDS;
  speedAsPercent = percent >= 0 ? percent : -percent;
}


//
// Get debug name
//  
const char* Motor::debugName()
{
  return "L298 Motor Control";
}

} // End Command Namespace

