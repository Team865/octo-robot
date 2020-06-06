#include "data_mover.h"
#include "net_interface.h"
#include "temperature_interface.h"
#include "wifi_ostream.h"

DataMover::DataMover(
  std::string deviceNameArg,
  std::shared_ptr<TempInterface> tempArg,
  std::shared_ptr<NetInterface> netArg
) : temp{ tempArg}, net{ netArg }
{
  deviceName = deviceNameArg;
  while ( deviceName.size() < 8 ) {
    deviceName.push_back(' ');
  }
}

void DataMover::packageData( const char *type, unsigned int data )
{
  (*net) << deviceName << " " << type << " " << data << "\n";
}

unsigned int DataMover::loop() 
{
  float t = temp->readTemperature();
  int t_int = t*10.0f;
  packageData("Temp", t_int );
  return 1000000;
}

