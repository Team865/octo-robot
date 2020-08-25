#include "command_sr04.h"
#include "wifi_debug_ostream.h"
#include "wifi_debug_ostream.h"

namespace Command{

const Time::DeviceTimeUS echoTimeout{ 4500 };

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

  //
  // The ECHO pin mode setup will be set by the hardware interface.  Echo is an
  // interrupt driven interrupt, so the support for it is a bit more complex
  // :(
  //
  // hwi->PinMode(pinEcho,  HW::PinIOMode::M_INPUT );
  //
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );
#endif
}


//
// We're in a state where we sent out an echo.  Process results.
//
// 1.  Process all events on the echo input pin that took place while we slept
// 1a. Record the start of the echo pulse 
// 2b. Record the end of the echo pulse
// 3.  Handle unsuccessful pulse events
// 4.  Figure out how long the echo pulse was
// 5.  Compute the distance using the time
// 
void SR04::handleAwaitingEcho()
{
  HW::IEvent& echoEvents = hwi->GetInputEvents( pinEcho );
  bool pulseUpSeen = false;
  bool pulseDownSeen = false;
  Time::DeviceTimeUS pulseUpTime;
  Time::DeviceTimeUS pulseDownTime;

  // 1.  Process all events on the echo input pin that took place while we slept
  //
  while( echoEvents.hasEvents() ) 
  {
    const Util::IPinEvent event   = echoEvents.read();
    const HW::PinState pinState   = event.first;
    const Time::DeviceTimeUS time = event.second;

    if ( !pulseUpSeen && pinState == HW::PinState::INPUT_HIGH ) 
    {
      // 1a. Record the start of the echo pulse
      // 
      pulseUpSeen = true;
      pulseUpTime = time;
    }
    if ( pulseUpSeen && !pulseDownSeen && pinState == HW::PinState::INPUT_LOW )
    {
      // 2b. Record the end of the echo pulse
      //
      pulseDownSeen = true;
      pulseDownTime = time;
    } 
  }

  // 3 Handle unsuccessful pulse events
  // 
  if ( !pulseDownSeen ) {
    net->get() << "RNG FAIL_NOECHO\n";
    mode = Mode::IDLE;
    distance = READING_FAILED;
    return;
  }

  // 4.  Figure out how long the echo pulse was
  //
  Time::DeviceTimeUS delay { pulseDownTime.get() - pulseUpTime.get() };
 
  // 5.  Compute the distance using the time
  //
  // distance in cm = usSinceEchoSent * .034 / 2 ( see header for details)
  // distance in mm = usSinceEchoSent * .34 / 2;
  // = usSinceEchoSent * .17
  // = usSinceEchoSent * 17 / 100;
  //
  distance = delay.get() * 17 / 100;
  net->get() << "RNG " << distance << "\n";
  mode = Mode::IDLE;
}

//
// Handle a request for a sonar rangle find.
//
// 1. Pulse the echo pin, which will trigger the SR04 range finder
// 2. Change mode to "waiting for echo"
// 
void SR04::handleSensorRequested()
{
  // 1. Pulse the echo pin, which will trigger the SR04 range finder
  //
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );
  auto start = hst->usSinceDeviceStart();
  while (hst->usSinceDeviceStart() - start < 3 );

  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_ON );
  start = hst->usSinceDeviceStart();
  while (hst->usSinceDeviceStart() - start < 11 );

  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );

  // 2. Change mode to "waiting for echo"
  //
  distance = READING_IN_PROGRESS;
  mode = Mode::AWAITING_ECHO;
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

  // Update at 50x a second
  return Time::TimeUS{ 20000 };
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

