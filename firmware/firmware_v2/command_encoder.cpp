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
  int low;
  int high;
  int val;

  // ==Low==
  hwi->WireBeginTransmission(I2C_ADRESS);
  hwi->WireWrite(_mag_lo);
  hwi->WireEndTransmission();
  hwi->WireRequestFrom(I2C_ADRESS, 1);

  while (hwi->WireAvailable() == 0)
  ;
  high = hwi->WireRead();

  // ==High==
  hwi->WireBeginTransmission(I2C_ADRESS);
  hwi->WireWrite(_mag_hi);
  hwi->WireEndTransmission();
  hwi->WireRequestFrom(I2C_ADRESS, 1);

  while (hwi->WireAvailable() == 0)
  ;
  high = hwi->WireRead();

  high = high << 8;
  val = high | low;

  (*debug) << val << '\n';

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

