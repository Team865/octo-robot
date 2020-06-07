#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "debug_interface.h"
#include "time_esp8266.h"

/*
  Based on code from:

  Udp NTP Client
  Get the time from a Network Time Protocol (NTP) time server
  Demonstrates use of UDP sendPacket and ReceivePacket
  For more on NTP time servers and the messages needed to communicate with them,
  see http://en.wikipedia.org/wiki/Network_Time_Protocol
  created 4 Sep 2010
  by Michael Margolis
  modified 9 Apr 2012
  by Tom Igoe
  updated for the ESP8266 12 Apr 2015
  by Ivan Grokhotkov
  This code is in the public domain.
*/


// local port to listen for UDP packets
constexpr unsigned int localPort = 2390;      
// NTP time stamp is in the first 48 bytes of the message
constexpr int NTP_PACKET_SIZE = 48; 

unsigned int TimeESP8266::secondsSince1970Raw() 
{
  IPAddress timeServerIP; 

  // Don't hardwire the IP address or we won't get the benefits of the pool.
  // Lookup the IP address for the host name instead 
  const char* ntpServerName = "time.nist.gov";

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb) {
    (*debug) << "no packet yet\n";
  } else {
    (*debug) << "got packet, length= " << cb << "\n";

    //buffer to hold incoming packets
    byte packetBuffer[ NTP_PACKET_SIZE];
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE);

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    const unsigned int highWord = word(packetBuffer[40], packetBuffer[41]);
    const unsigned int lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    const unsigned int secsSince1900 = highWord << 16 | lowWord;
    (*debug) << "seconds since Jan 1 1900 = " << secsSince1900 << "\n";

    constexpr unsigned int secsIn70Years = 2208988800U;
    const unsigned int secsSince1970 = secsSince1900 - secsIn70Years;
    return secsSince1970;
  }
  return 0;
}

unsigned int TimeESP8266::secondsSince1970()
{
  unsigned int secsSince1970 = 0;
  for ( int i = 0; i < 5 && secsSince1970 == 0; ++i ) {
    secsSince1970 = secondsSince1970Raw();
  }
  return secsSince1970;
}

Time::DeviceTimeMS TimeESP8266::msSinceDeviceStart() 
{
  return millis();
}

TimeESP8266::TimeESP8266( std::shared_ptr<DebugInterface> debugArg ) :
  debug{ debugArg } 
{
  (*debug) << "Starting UDP\n";
  udp.begin(localPort);
  (*debug) << "Local port: " << udp.localPort() << "\n";
}

// send an NTP request to the time server at the given address
void TimeESP8266::sendNTPpacket(IPAddress& address) {
  //buffer to hold outgoing packets
  byte packetBuffer[ NTP_PACKET_SIZE];

  (*debug) << "sending NTP packet...\n";
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

