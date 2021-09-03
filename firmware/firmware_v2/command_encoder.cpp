#include "command_encoder.h"
#include "wifi_debug_ostream.h"

namespace Command{

Encoder::Encoder( 
  std::shared_ptr<HW::I> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg,
  int i2cBusArg
) :
    hwi { hwiArg }, debug { debugArg }, net { netArg }, i2cBus { i2cBusArg },
  position{ 0 }
{

}


Time::TimeUS Encoder::execute() 
{
  int low;
  int high;
  int val;

  // ==Low==
  //(*debug) << "Starting Low " << i2cBus << "\n";
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _mag_lo);
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2cBus) == 0)
  ;
  low = hwi->WireRead(i2cBus);

  //(*debug) << "Ending Low " << i2cBus << "\n";

  // ==High==
  //(*debug) << "Starting High\n";
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _mag_hi);
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2cBus) == 0)
  ;
  high = hwi->WireRead(i2cBus);

  high = high << 8;
  val = high | low;

  position = val;

  //(*debug) << i2cBus << ": " << high << ", " << low << "\n";
  (*debug) << i2cBus << ": " << position << "\n";

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
  //(*debug) << "Getting position\n";
  return position;
}

int Encoder::getSpeed()
{
  return 0;
}

} // End Command Namespace

