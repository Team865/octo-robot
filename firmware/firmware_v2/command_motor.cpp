#include "command_motor.h"
#include "wifi_debug_ostream.h"

namespace Command{

//=======================================================================

MotorHardware::MotorHardware(
  std::shared_ptr<HW::I> hwiArg, 
  HW::Pin pin0Arg,  
  HW::Pin pin1Arg 
) :
  hwi{hwiArg}, pin0{ pin0Arg }, pin1{ pin1Arg} 
{
  // Configure hardware pins for output
  hwi->PinMode(pin0,  HW::PinIOMode::M_OUTPUT );
  hwi->PinMode(pin1,  HW::PinIOMode::M_OUTPUT );

  // Set the motor to off
  setMotorStop();
}

void MotorHardware::setMotorForward()
{
  // Forward, one input on, the other input off
  hwi->DigitalWrite( pin0, HW::PinState::MOTOR_POS );
  hwi->DigitalWrite( pin1, HW::PinState::MOTOR_NEG );
}

void MotorHardware::setMotorBackward()
{
  // Backward, one input off, the other input on
  hwi->DigitalWrite( pin0, HW::PinState::MOTOR_NEG );
  hwi->DigitalWrite( pin1, HW::PinState::MOTOR_POS );
}

void MotorHardware::setMotorStop()
{
  // Stopped,  Both inputs off.
  hwi->DigitalWrite( pin0, HW::PinState::MOTOR_NEG );
  hwi->DigitalWrite( pin1, HW::PinState::MOTOR_NEG );
}

//=======================================================================

MotorState::MotorState(
  std::shared_ptr<HW::I> hwiArg, 
  HW::Pin pin0Arg,  
  HW::Pin pin1Arg 
) :
  motorHardware{ hwiArg, pin0Arg, pin1Arg }, lastPulse{ Pulse::NONE }
{
}

//
// Set the motor controller output as per pulse
// 
void MotorState::doPulse( MotorState::Pulse pulse )
{
  if ( lastPulse == pulse ) {
    return;
  }
  lastPulse = pulse;

  switch( pulse ) 
  {
    case Pulse::FORWARD:
      motorHardware.setMotorForward();
      break;
    case Pulse::BACKWARD:
      motorHardware.setMotorBackward();
      break;
    case Pulse::NONE:
      motorHardware.setMotorStop();
      break;
  }
}

//=======================================================================

Motor::Motor( 
  std::shared_ptr<HW::I> hwiArg, 
  HW::Pin pin0Arg,  
  HW::Pin pin1Arg 
) :
  motorState{ hwiArg, pin0Arg, pin1Arg }, 
  dir { Motor::Dir::FORWARD }, speedAsPercent{ 0 }, counter{ 0 }
{
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
    motorState.doPulse( MotorState::Pulse::NONE );
  }
  else {
    if ( dir == Motor::Dir::FORWARD ) {
      motorState.doPulse( MotorState::Pulse::FORWARD );
    }
    else {
      motorState.doPulse( MotorState::Pulse::BACKWARD );
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

