#include "command_encoder.h"
#include "wifi_debug_ostream.h"
#include <cmath>

namespace Command{

Encoder::Encoder( 
  std::shared_ptr<HW::I> hwiArg, 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg,
  std::shared_ptr<Time::HST> hstArg,
  int i2cBusArg
) :
    hwi { hwiArg }, debug { debugArg }, net { netArg }, i2cBus { i2cBusArg },
  position{ 0 }, hst{ hstArg }
{
  constexpr int _stat = 0xb;
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _stat );
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2cBus) == 0);
  const int stat = hwi->WireRead(i2cBus);
  if ( stat & 0x20 ) {
    int strength = 1;
    if ( stat & 0x10 ) { strength = 0; } // too weak
    if ( stat & 0x08 ) { strength = 2; } // too strong
    constexpr std::string_view strengthText[] = {"too weak", "just right", "too string" }; 

    (*debug) << "Encoder " << i2cBus << " magnet detected. strength "  << strengthText[strength] << "\n";
  }
  else {
    (*debug) << "Encoder " << i2cBus << " magnet not detected\n";
  }
  
  constexpr int _agc = 0x1a;
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _agc );
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2cBus) == 0);
  const int agc = hwi->WireRead(i2cBus);

  (*debug) << "AGC " << agc << "\n";

  constexpr int _mag= 0x1b;
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _mag);
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 2);

  while (hwi->WireAvailable(i2cBus) == 0);
  const int mag = hwi->WireRead2(i2cBus);

  speed_accumulate = 0;
  speed_accumulate_start = hst->msSinceDeviceStart(); 

  (*debug) << "mag " << mag << "\n";
}


Time::TimeUS Encoder::execute() 
{
  int low;
  int high;

  // ==Low==
  //(*debug) << "Starting Low " << i2cBus << "\n";
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _raw_ang_lo );
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2cBus) == 0);
  low = hwi->WireRead(i2cBus);

  //(*debug) << "Ending Low " << i2cBus << "\n";

  // ==High==
  //(*debug) << "Starting High\n";
  hwi->WireBeginTransmission(i2cBus, I2C_ADRESS);
  hwi->WireWrite(i2cBus, _raw_ang_hi );
  hwi->WireEndTransmission(i2cBus);
  hwi->WireRequestFrom(i2cBus, I2C_ADRESS, 1);

  while (hwi->WireAvailable(i2cBus) == 0)
  ;
  high = hwi->WireRead(i2cBus);

  high = high << 8;
  const int raw_position = high | low;

  const int position_dif = ( raw_position - last_raw_position );
  int delta_position = 0;

  if ( position_dif < -2000 ) {
    // probably cycled from something like 3900 to 100
    // That case would give a number like -3800
    // After adding 4096 we get 296 (i.e., positive incr) 
    delta_position = position_dif + 4096;
  }
  else if ( position_dif > 2000 ) {
    // probably cycled from something like 100 to 3900
    // That case would give a number like +3800
    // After subtracting 4096 we get -296 (i.e., negative incr) 
    delta_position = position_dif - 4096;
  } 
  else {
    // A typical case, like 2000 to 2200, or 2200 to 2000.
    delta_position = position_dif;
  }
  position += delta_position;
  speed_accumulate += delta_position;
  ++speed_count;
  if (( speed_count % 10 ) == 0 )
  {
    Time::DeviceTimeMS current = hst->msSinceDeviceStart();
    //(*debug) << speed_accumulate << " " << ( current - speed_accumulate_start ) << "\n";
    long long speed_tmp = speed_accumulate;
    speed_tmp = speed_tmp * 1000 / (current - speed_accumulate_start); 
    speed = speed_tmp;
    speed_accumulate = 0;
    speed_accumulate_start = current;
  }

  last_raw_position = raw_position;


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
  return position;
}

int Encoder::getSpeed()
{
  //return last_raw_position;
  return speed;
}

} // End Command Namespace
