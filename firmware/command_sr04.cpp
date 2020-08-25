#include "command_sr04.h"
#include "wifi_debug_ostream.h"
#include "wifi_debug_ostream.h"
#include <algorithm>    // for std::sort

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
  mode{ Mode::IDLE }
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
void SR04::processPulseResult()
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
  samples.at( currentSample ) = delay.get() * 17 / 100;
}

//
// Send a sonar pulse out
//
// 1. Reset the Trigger pin for >2ms
// 2. Activate the Trigger pin for 10us
// 3. Reset the Trigger pin again
// 
void SR04::sendPulse()
{
  // 1. Reset the Trigger pin for >2ms
  //
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );
  auto start = hst->usSinceDeviceStart();
  while (hst->usSinceDeviceStart() - start < 3 );

  // 2. Activate the Trigger pin for 10us
  //
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_ON );
  start = hst->usSinceDeviceStart();
  while (hst->usSinceDeviceStart() - start < 13 );

  // 3. Reset the Trigger pin again
  //
  hwi->DigitalWrite( pinTrig, HW::PinState::ECHO_OFF );
}

Time::TimeUS SR04::execute() 
{
  if ( mode == Mode::DOING_READING ) {
    if ( currentSample != -1 ) {
      // If we're not doing the first reading there should be a pulse result
      SR04::processPulseResult();
    }
    if ( currentSample < numSamples-1 ) {
      // If we're not doing the last reading then we need to send a pulse
      SR04::sendPulse();
    }
    ++currentSample;
    if ( currentSample == numSamples ) {
      //
      // Output the medium and reset the state.
      //
      // The medium sample was used because the range finder I tested seems
      // to occasionally produce a very inaccurate result.  i.e.,  I'd get
      // readings like 1005, 998, 500 when the object was about 10cm away.
      //
      std::sort( samples.begin(), samples.end() );
      net->get() << "RNG " << samples[numSamples/2] << "\n";
      currentSample = -1;
      mode = Mode::IDLE;
    }
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
    mode = Mode::DOING_READING;
    currentSample = -1;
  }
}

} // End Command Namespace

