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
  int i2c_bus = 0;

  // ==Low==
  hwi->WireBeginTransmission(i2c_bus, I2C_ADRESS);
  hwi->WireWrite(i2c_bus, _mag_lo);
  hwi->WireEndTransmission(i2c_bus);
  hwi->WireRequestFrom(i2c_bus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2c_bus) == 0)
  ;
  high = hwi->WireRead(i2c_bus);

  // ==High==
  hwi->WireBeginTransmission(i2c_bus, I2C_ADRESS);
  hwi->WireWrite(i2c_bus, _mag_hi);
  hwi->WireEndTransmission(i2c_bus);
  hwi->WireRequestFrom(i2c_bus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2c_bus) == 0)
  ;
  high = hwi->WireRead(i2c_bus);

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

