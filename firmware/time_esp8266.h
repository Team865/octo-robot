#ifndef __TIME_ESP8266_H__
#define __TIME_ESP8266_H__

#include <WiFiUdp.h>
#include "time_interface.h"

class IPAddress;

class TimeESP8266: public Time::Interface {
  public:

  TimeESP8266( std::shared_ptr<DebugInterface> debugArg );

  virtual Time::RealTimeS secondsSince1970() override final;

  private:
  std::shared_ptr<DebugInterface> debug;

  // A UDP instance to let us send and receive packets over UDP
  WiFiUDP udp;

  void sendNTPpacket(IPAddress& address );
  unsigned int secondsSince1970Raw();
};

#endif

