#include "temperature_dh11.h"

TempDH11::TempDH11( int pin )
{
  dht = std::unique_ptr<DHT>( new DHT( pin, DHT11 ) );
}

float TempDH11::readTemperature()
{
  return dht->readTemperature(false);
}

float TempDH11::readHumidity()
{
  return dht->readHumidity();
}

