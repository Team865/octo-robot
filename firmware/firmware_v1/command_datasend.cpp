#include "command_datasend.h"
#include "wifi_debug_ostream.h"

namespace Command{

DataSend::DataSend( 
  std::shared_ptr<DebugInterface>     debugArg, 
  std::shared_ptr<NetInterface>       netArg, 
  std::shared_ptr<Command::Encoder>   encoderLArg,
  std::shared_ptr<Command::Encoder>   encoderRArg,
  std::shared_ptr<Command::SR04>      rangeFinderArg,
  std::shared_ptr<HW::I>              hwiArg
) :
  debug { debugArg }, 
  net { netArg }, 
  encoderL{ encoderLArg },
  encoderR{ encoderRArg },
  rangeFinder{ rangeFinderArg },
  hwi{ hwiArg}
{
}

//
// Standard execute method
//
Time::TimeUS DataSend::execute() 
{
  timesCalled++;

  if ( isOutputting ) {
    net->get() << "ENL " << encoderL->getPosition() << " " << encoderL->getSpeed() << "\n";  
    net->get() << "ENR " << encoderR->getPosition() << " " << encoderR->getSpeed() << "\n";
    net->get() << "RNG " << rangeFinder->getLastSensorReading() << "\n";
  }
  rangeFinder->sensorRequest();

#ifndef OCTO_ESP8266_DEBUG
  updateLEDs();
#endif

  return Time::TimeMS( 100 );  // 10 updates / second
}

//
// 1. Use the Sonar Range finder current distance for the center color
// 2. Use the Left encoder speed for the left color.
// 3. Use the Right encoder speed for the right color.
// 4. Blend Colors
// 5. Update
//
void DataSend::updateLEDs() 
{
  unsigned left[3];
  unsigned right[3];
  unsigned center[3];

  constexpr size_t R = 0;
  constexpr size_t G = 1;
  constexpr size_t B = 2;

  //
  // 1. Use the Sonar Range finder current distance for the center color
  // 
  const int range = rangeFinder->getLastSensorReading();
  if ( range >= 0 ) { 
    lastValidSensorReading = range;
  }
  if ( lastValidSensorReading < 0 ) {
    center[ R ] = 0;
    center[ G ] = 255;
    center[ B ] = 0;
  }
  else {
    // Display a color from red to blue, depending on how close the wall
    // is.  Red = closer (hotter), blue = further (colder).  For the purpose
    // of the display, go full blue at 500mm, or 50cm.
    constexpr int maxRangeForDisplayPurposes = 500;
    const int drange = std::min( lastValidSensorReading , maxRangeForDisplayPurposes );
    // Full Blue:  drange = maxRangeForDisplayPurposes 
    // Full Red:   drange = 0
    center[ B ] = 255 * drange / maxRangeForDisplayPurposes;
    center[ R ] = 255 - center[B];
    center[ G ] = 0;
  }
 
  //
  // 2. Use the Left encoder speed for the left color.
  // 
  int leftMag = -encoderL->getSpeed();
  if ( leftMag >= 0 )
  {
    left[ R ] = 0;
    left[ G ] = std::min(leftMag,120000)*255/120000;
    left[ B ] = 255 - left[G];
  }
  else {
    left[ R ] = std::min(-leftMag,120000)*255/120000;
    left[ G ] = 0;
    left[ B ] = 255 - left[R];
  }

  //
  // 3. Use the Right encoder speed for the right color.
  // 
  int rightMag = encoderR->getSpeed();
  if ( rightMag >= 0 )
  {
    right[ R ] = 0;
    right[ G ] = std::min(rightMag,120000)*255/120000;
    right[ B ] = 255 - right[G];
  }
  else {
    right[ R ] = std::min(-rightMag,120000)*255/120000;
    right[ G ] = 0;
    right[ B ] = 255 - right[R];
  }

  //
  // 4. Blend Colors
  // 
  for ( size_t i = 0; i < 4; ++i ) 
  {
    unsigned leftSide[3];
    unsigned rightSide[3];
    //
    // 0.8 fixed point for % from center and edge.
    //
    unsigned perCenter = i * 256 / 3;
    unsigned perEdge   = 256 - perCenter;

    for ( size_t c = 0; c < 3; ++c ) 
    {
        leftSide [c] = std::min((left [c] * perEdge + center[c] * perCenter ) / 256,255u); 
        rightSide[c] = std::min((right[c] * perEdge + center[c] * perCenter ) / 256,255u); 
    }
    hwi->LEDSet( 7-i, leftSide [ R ]/4, leftSide [ G ]/4, leftSide [ B ]/4 );
    hwi->LEDSet(   i, rightSide[ R ]/4, rightSide[ G ]/4, rightSide[ B ]/4 );
  } 
  //
  // 5. Update
  // 
  hwi->LEDUpdate();
}


//
// Get debug name
//  
const char* DataSend::debugName()
{
  return "Data Sender";
}

void DataSend::setOutput( bool isOutputtingArg )
{
  isOutputting = isOutputtingArg;
}

} // End Command Namespace

