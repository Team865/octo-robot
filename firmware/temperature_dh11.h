#ifndef __TEMP_DH11_H__
#define __TEMP_DH11_H__

#include <memory>
#include "temperature_interface.h"
#include "DHT.h"

constexpr int DhtPin=5;

class TempDH11: public TempInterface {
  public:

  TempDH11( int pin );

  float readTemperature() override final;
  float readHumidity() override final;

  private:

  std::unique_ptr<DHT> dht;
};

#endif


