#include "command_encoder.h"
#include "wifi_debug_ostream.h"

namespace Command{

Encoder::Encoder( 
  std::shared_ptr<HWI> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg, 
  HWI::Pin pin0Arg,  
  HWI::Pin pin1Arg 
) :
  hwi { hwiArg }, debug { debugArg }, net { netArg }, 
  pin0{ pin0Arg }, pin1{ pin1Arg},
  position{ 0 }
{
  // Configure hardware pins for output
  hwi->PinMode(pin0,  HWI::PinIOMode::M_INPUT );
  hwi->PinMode(pin1,  HWI::PinIOMode::M_INPUT );

  lastGreyCode = getGreyCode();
}

//
// Background reading:
//
// https://www.handsontec.com/dataspecs/module/Rotary%20Encoder.pdf
//
// Grey code 101
// =============
// 
// Suppose the last thing we read from the encoder was the following:
//
// pin0State = false;
// pin1State = false;
//
// i.e., both of the encoders are off.
//
// When the encoder transitions to a new state, as long as we're sampling
// fast enough, one of those two states will go high, but not both of them.
//
// If we encode pin0State and pin1State as a binary numbers,  i.e,
//
// greycode = 0;
//
// if ( pin0State ) { greyCode += 1 }
// if ( pin1State ) { greyCode += 2 }
// 
// Then the gransitions out of "value 0" ( both pinstates false ) are
// 
// 1    ( pin0State is true,  pin1State is false )
// 2    ( pin0State is false, pin1State is true )
//
// Each of these two states represents a rotation.  We can encode the
// what a 0 to something else transition means like this
//
// 0 -> 0     No Change
// 0 -> 1     Rotate Clockwise
// 0 -> 2     Rotate Counter Clockwise
// 0 -> 3     Illegal!
// 
// If you start with a grey code of 1, pin0State = true, pin1State = false
//
// 1 -> 0     Rotate Counter Clockwise (we just reverse direction back to 0)
// 1 -> 1     No change
// 1 -> 2     Illegal
// 1 -> 3     Rotate Clockwise
//
// Continuing clockwise, 
//
// 3 -> 0     Illegal
// 3 -> 1     Rotate Counter Clockwise (back where we just came from)
// 3 -> 2     Rotate Clockwise
// 3 -> 3     No change
//
// And finally
//
// 2 -> 0     Clockwise Rotateion
// 2 -> 1     Illegal
// 2 -> 2     No Change
// 2 -> 3     Counter Clockwise
//
//  

Encoder::Action Encoder::greyCodeActionTable[ 4 ][ 4 ] = {
  // Start position is 0
  {
    Action::NO_ACTION,      // 0 -> 0, or (binary) { 0, 0 } -> { 0, 0 } 
    Action::CW_ROTATION,    // 0 -> 1, or (binary) { 0, 0 } -> { 0, 1 } 
    Action::CCW_ROTATION,   // 0 -> 2, or (binary) { 0, 0 } -> { 1, 0 } 
    Action::ILLEGAL         // 0 -> 3, or (binary) { 0, 0 } -> { 1, 1 }
  },
  // Start position is 1
  {
    Action::CCW_ROTATION,   // 1 -> 0, or (binary) { 0, 1 } -> { 0, 0 } 
    Action::NO_ACTION,      // 1 -> 1, or (binary) { 0, 1 } -> { 0, 1 } 
    Action::ILLEGAL,        // 1 -> 2, or (binary) { 0, 1 } -> { 1, 0 } 
    Action::CW_ROTATION     // 1 -> 3, or (binary) { 0, 1 } -> { 1, 1 }
  },
  // Start position is 2
  {
    Action::CW_ROTATION,    // 2 -> 0, or (binary) { 1, 0 } -> { 0, 0 } 
    Action::ILLEGAL,        // 2 -> 1, or (binary) { 1, 0 } -> { 0, 1 } 
    Action::NO_ACTION,      // 2 -> 2, or (binary) { 1, 0 } -> { 1, 0 } 
    Action::CCW_ROTATION    // 2 -> 3, or (binary) { 1, 0 } -> { 1, 1 }
  },
  // Start position is 3
  {
    Action::ILLEGAL,        // 2 -> 0, or (binary) { 1, 1 } -> { 0, 0 } 
    Action::CCW_ROTATION,   // 2 -> 1, or (binary) { 1, 1 } -> { 0, 1 } 
    Action::CW_ROTATION,    // 2 -> 2, or (binary) { 1, 1 } -> { 1, 0 } 
    Action::NO_ACTION       // 2 -> 3, or (binary) { 1, 1 } -> { 1, 1 }
  }
};

unsigned int Encoder::Encoder::getGreyCode()
{
  const bool pin0State = ( hwi->DigitalRead( pin0 ) == HWI::PinState::INPUT_HIGH );
  const bool pin1State = ( hwi->DigitalRead( pin1 ) == HWI::PinState::INPUT_HIGH );
  return ( pin1State << 1 ) | pin0State;
}


Time::TimeUS Encoder::execute() 
{
  const unsigned int greyCode = getGreyCode();
//  *net << greyCode << "\n";
 
  const Action action = greyCodeActionTable[ lastGreyCode ][ greyCode ];
  lastGreyCode = greyCode;

  switch ( action ) 
  {
    case Action::CW_ROTATION:
      position++;
      break;
    case Action::CCW_ROTATION:
      position--;
      break;
    case Action::NO_ACTION:
      break;    // don't do anything
    case Action::ILLEGAL:
      break;    // what can we do?  :(  
  }

  // Duration Query...
  //
  // How fast can we run this?
  // 
  // Let's say we want to use a maximum of 4% of the CPU budget servicing this
  // 
  // - The ESP8266 clock runs at 80Mhz, so we have 3.2 million cycles
  // - Let's say each execute call is 500 cycls.  That's 6400 updates / sec
  // - Assume that means we can deal with 6400 encoder state changes / sec
  // - The encoder has 80 states, so that's 80 rotations / sec
  // - If we gear the encode down 5x from the wheel, 16 wheel rotations / sec
  // - The wheel is 66mm, or 20.7cm in diameter.
  // - so top speed = 20.7cm wheel diamater x 16 wheel rotations / sec;
  // - or 3.31 m / sec
  // - or 12km / hour, a clippy running pace.
  //
  // - TODO - adjust as needed.
  //
  return Time::TimeUS( 156 );
}

//
// Get debug name
//  
const char* Encoder::debugName()
{
  return "Encoder";
}

int Encoder::getPosition()
{
  return position;
}

} // End Command Namespace

