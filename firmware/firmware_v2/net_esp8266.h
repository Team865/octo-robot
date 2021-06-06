#ifndef __WifiInterfaceEthernet_H__
#define __WifiInterfaceEthernet_H__

#include <string>
#include <memory>
#include <ios>
#include <ESP8266WiFi.h>
#include "wifi_ostream.h"
#include "wifi_secrets.h"
#include "debug_interface.h"

class WifiOstream;

class WifiDebugOstream;

class WifiConnectionEthernet: public NetConnection {

  public:

  struct category: beefocus_tag {};
  using char_type = char;

  WifiConnectionEthernet( std::shared_ptr<DebugInterface> debugArg )
    : debug{ debugArg }
  {
    reset();
  }

  ~WifiConnectionEthernet()
  {
    reset();
  }

  void reset( void ) 
  { 
    if (m_connectedClient)
    {
      m_connectedClient.stop();
    }
  }

  void writePushImpl( NetPipe& pipe );

  Time::TimeUS execute();

  void initConnection( WiFiServer &server );
  operator bool( void ) override {
    return m_connectedClient;
  }

  private:

  WiFiClient m_connectedClient;
  std::shared_ptr<DebugInterface> debug;
};

/// @brief Interface to the client
///
/// This class's one job is to provide an interface to the client.
///
class WifiInterfaceEthernet: public NetInterface {
  public:

  WifiInterfaceEthernet( std::shared_ptr<DebugInterface> debugLog );

  ~WifiInterfaceEthernet()
  {
    reset();
  }

  void reset( void );

  const char* debugName() override
  {
    return "ConnectWatch";
  }
  //std::unique_ptr<NetConnection> connect( const std::string& location, unsigned int port );
  NetConnection& get() override
  {
    assert( defaultConnection );
    return *defaultConnection;
  }

  std::shared_ptr<NetConnection> getShared() {
    return defaultConnection;
  }

  Time::TimeUS execute();
  
  private:

  void handleNewConnections();

  // Make a CI Test to lock these defaults in?
  static constexpr const char* ssid = WifiSecrets::ssid; 
  static constexpr const char* password = WifiSecrets::password;
  static constexpr const char* hostname = WifiSecrets::hostname;
  const uint16_t tcp_port{4999};

  std::shared_ptr<DebugInterface> log;
  std::shared_ptr<WifiConnectionEthernet> defaultConnection;

  WiFiServer m_server{tcp_port};
};

#endif
