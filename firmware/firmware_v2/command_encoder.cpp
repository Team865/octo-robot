#include "command_encoder.h"
#include "wifi_debug_ostream.h"

namespace Command{

Encoder::Encoder( 
  std::shared_ptr<HW::I> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg
) :
    hwi { hwiArg }, debug { debugArg }, net { netArg },
  position{ 0 }
{
}


Time::TimeUS Encoder::execute() 
{
  // Run this about 100 times a second.
  //
  return Time::TimeUS( 10000 );
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
  return 0;
}

int Encoder::getSpeed()
{
  return 0;
}


} // End Command Namespace

