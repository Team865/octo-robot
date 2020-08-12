#include "command_sr04.h"
#include "wifi_debug_ostream.h"

namespace Command{

const Time::DeviceTimeUS echoTimeout{ 5882 };

// Update at 100x a second when in idle mode
const Time::TimeUS idleReschedule{ 10000 };

// Update at 20us when taking a sensor reading
const Time::TimeUS activeReschedule{ 20 };

SR04::SR04( 
  std::shared_ptr<HWI> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg, 
  std::shared_ptr<Time::HST> hstArg, 
  HWI::Pin pinTrigArg,  
  HWI::Pin pinEchoArg 
) :
  hwi { hwiArg }, debug { debugArg }, net { netArg }, 
  hst{ hstArg },
  pinTrig{ pinTrigArg }, pinEcho{ pinEchoArg },
  mode{ Mode::IDLE },
  distance{ READING_FAILED }
{
  // Configure hardware pins for output
  hwi->PinMode(pinTrig,  HWI::PinIOMode::M_OUTPUT );
  hwi->PinMode(pinEcho,  HWI::PinIOMode::M_INPUT );
  hwi->DigitalWrite( pinTrig, HWI::PinState::ECHO_OFF );
}


void SR04::handleAwaitingEcho()
{
  Time::DeviceTimeUS time = hst->usSinceDeviceStart();
  Time::DeviceTimeUS usSinceEchoSent{ time - echoSentAt };
  if ( usSinceEchoSent > echoTimeout  ) {   // timeout
    // TODO - retry bad readings?
    mode = Mode::IDLE;
    distance = READING_FAILED;
    return;
  }

  if ( hwi->DigitalRead( pinEcho ) == HWI::PinState::INPUT_HIGH )
  {
    //
    // The pulse came in sometime between now and the last time we checked
    // So take the average as the best guess.
    //
    // TODO - We're hoping for 20us delays, but the actual delays may be
    //        longer if we have to respond to something like a network 
    //        event.  The tail latency there can be over 100us.  Maybe
    //        throw out readings with a long delay and restart.
    //
    Time::DeviceTimeUS avgTime { ( usSinceEchoSent - lastCheck ) / 2};
    //
    // distance in cm = usSinceEchoSent * .034 / 2 ( see header for details)
    // distance in mm = usSinceEchoSent * .34 / 2;
    // = usSinceEchoSent * .17
    // = usSinceEchoSent * 17 / 100;
    //
    distance = avgTime.get() * 17 / 100;
    mode = Mode::IDLE;
    return;
  }
  lastCheck = usSinceEchoSent;  // record the last time we checked
}

void SR04::handleSensorRequested()
{
  // Clear the echo pin for at least 2us
  hwi->DigitalWrite( pinTrig, HWI::PinState::ECHO_OFF );
  Time::DeviceTimeUS startPinDown = hst->usSinceDeviceStart();
  while ( hst->usSinceDeviceStart() - startPinDown < 3 );

  // Pulse the echo pin back on for 10us
  hwi->DigitalWrite( pinTrig, HWI::PinState::ECHO_ON );
  while ( hst->usSinceDeviceStart() - startPinDown < 13 );
  hwi->DigitalWrite( pinTrig, HWI::PinState::ECHO_OFF );

  // Sent start times
  echoSentAt = startPinDown+13;
  lastCheck = echoSentAt;
  
  // Set mode and state variables
  mode = Mode::AWAITING_ECHO;
  distance = READING_IN_PROGRESS;
}

Time::TimeUS SR04::execute() 
{
  switch( mode ) {
    case Mode::IDLE:
      break;
    case Mode::SENSOR_REQUESTED:
      handleSensorRequested();
      break;
    case Mode::AWAITING_ECHO:
      handleAwaitingEcho();
      break;
    default:
      break;
  }

  return ( mode == Mode::IDLE ) ? idleReschedule : activeReschedule; 
}

//
// Get debug name
//  
const char* SR04::debugName()
{
  return "SR04";
}

void SR04::sensorRequest() 
{
  if ( mode == Mode::IDLE ) {
    distance = READING_IN_PROGRESS;
    mode = Mode::SENSOR_REQUESTED;
  }
}

} // End Command Namespace

