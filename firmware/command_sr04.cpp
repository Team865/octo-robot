#include "command_sr04.h"
#include "wifi_debug_ostream.h"
#include "wifi_debug_ostream.h"

namespace Command{

const Time::DeviceTimeUS echoTimeout{ 4500 };

// Update at 100x a second when in idle mode
const Time::TimeUS idleReschedule{ 10000 };

// Update at 20us when taking a sensor reading
const Time::TimeUS activeReschedule{ 20 };

SR04::SR04( 
  std::shared_ptr<HW::I> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg, 
  std::shared_ptr<Time::HST> hstArg, 
  HW::Pin pinTrigArg,  
  HW::Pin pinEchoArg 
) :
  hwi { hwiArg }, debug { debugArg }, net { netArg }, 
  hst{ hstArg },
  pinTrig{ pinTrigArg }, pinEcho{ pinEchoArg },
  mode{ Mode::IDLE },
  distance{ READING_FAILED }
{
  // Configure hardware pins for output
#ifndef OCTO_ESP8266_DEBUG
  hwi->PinMode(pinTrig,  HW::PinIOMode::M_OUTPUT );
  hwi->PinMode(pinEcho,  HW::PinIOMode::M_INPUT );
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );
#endif
}


void SR04::handleAwaitingEcho()
{
  Time::DeviceTimeUS time = hst->usSinceDeviceStart();
  Time::DeviceTimeUS usSinceEchoSent{ time - echoSentAt };
 
  if ( usSinceEchoSent > echoTimeout  ) {   // timeout
    // TODO - retry bad readings?
    net->get() << "RANGE FAIL NOECHO " << usSinceEchoSent.get() << "\n";
    mode = Mode::IDLE;
    distance = READING_FAILED;
    return;
  }

  if ( hwi->DigitalRead( pinEcho ) == HW::PinState::INPUT_LOW )
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
    Time::DeviceTimeUS avgTime { ( usSinceEchoSent.get() + lastCheck.get() ) / 2};
    unsigned int error = (usSinceEchoSent- lastCheck) / 2;
 
    //
    // distance in cm = usSinceEchoSent * .034 / 2 ( see header for details)
    // distance in mm = usSinceEchoSent * .34 / 2;
    // = usSinceEchoSent * .17
    // = usSinceEchoSent * 17 / 100;
    //
    distance = avgTime.get() * 17 / 100;
    unsigned int errorDistance = error * 17 / 100;
    net->get() << "RANGE " << distance << " ERROR " << errorDistance << "\n";
    mode = Mode::IDLE;
    return;
  }
  lastCheck = usSinceEchoSent;  // record the last time we checked
}

void SR04::handleSensorRequested()
{
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );
  auto start = hst->usSinceDeviceStart();
  while (hst->usSinceDeviceStart() - start < 3 );

  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_ON );
  start = hst->usSinceDeviceStart();
  while (hst->usSinceDeviceStart() - start < 11 );

  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );

  start = hst->usSinceDeviceStart();
  while( hwi->DigitalRead( pinEcho ) == HW::PinState::INPUT_LOW ) 
  {
    // It looks like it takes about 500us to get a pulse out.
    if ( hst->usSinceDeviceStart() - start > 1000 ) 
    {
      net->get() << "RANGE FAIL TRIG TIMEOUT\n";
      mode = Mode::IDLE;
      distance = READING_FAILED;
      return;
    }
  }

  mode = Mode::AWAITING_ECHO;
  distance = READING_IN_PROGRESS;

  echoSentAt = hst->usSinceDeviceStart();
  lastCheck =  Time::DeviceTimeUS{0};
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

