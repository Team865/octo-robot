#ifndef __NetInterface_H__
#define __NetInterface_H__

#include <string>
#include <string>
#include <memory>
#include "action_interface.h"
#include "hardware_interface.h"
#include "debug_interface.h"

class WifiOstream;
class WifiDebugOstream;

class NetConnection {
  public:

  virtual ~NetConnection()
  {
  }

  struct category : public beefocus_tag {};
  using char_type = char;

  virtual bool getString( std::string& string )=0;
  /// @brief Is the connection good?
  virtual operator bool( void ) = 0;
  /// @brief Closes the connection
  virtual void reset( void ) = 0;
  virtual std::streamsize write( const char_type* s, std::streamsize n ) = 0;
  virtual void flush() = 0;
};

/// @brief Interface to the client
///
/// This class's one job is to provide an interface to the client.
///
/// TODO - make it an action?
///
class NetInterface: public Action::Interface {
  public:

  struct category : public beefocus_tag {};
  using char_type = char;

  NetInterface()
  {
  }
  virtual ~NetInterface()
  {
  }

  virtual bool getString( std::string& string ) = 0;
  virtual std::streamsize write( const char_type* s, std::streamsize n ) = 0;
  virtual void flush() = 0;
  virtual std::unique_ptr<NetConnection> connect( const std::string& location, unsigned int port ) = 0;

  private:
};


#endif

