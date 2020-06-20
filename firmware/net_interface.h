#ifndef __NetInterface_H__
#define __NetInterface_H__

#include <string>
#include <string>
#include <memory>
#include <assert.h>
#include "command_base.h"
#include "hardware_interface.h"
#include "debug_interface.h"
#include "util_pipe.h"

class WifiOstream;
class WifiDebugOstream;

using NetPipe = Util::Pipe<char, 1024>;

class NetConnection {
  public:

  struct category : public beefocus_tag {};
  using char_type = char;

  NetConnection() :
    writeBuffer{ [this]( NetPipe& pipe) { writePush(pipe);} },
    readBuffer { [this]( NetPipe& pipe) { readPush(pipe);} }
  {
  }

  virtual ~NetConnection()
  {
  }

  void writePush(NetPipe &pipe)
  {
    char opps = pipe.getChar();
  }

  void readPush(NetPipe &)
  {
  }

  std::streamsize write( const char_type* s, std::streamsize n )
  {
    for ( std::streamsize index = 0; index < n; ++index )
    {
      writeBuffer.putChar( s[index] );
    }
    return n;
  }

  virtual Time::TimeUS execute() = 0;

  bool getString( std::string& string )
  {
    for (;; ) {
      char c = readBuffer.getChar();
      if ( c == 0 ) { break; }
      if ( c == '\n' ) {
        return true;
      }
      string.push_back( c );
    }
    return false;
  }


  /// @brief Is the connection good?
  virtual operator bool( void ) = 0;
  /// @brief Closes the connection
  virtual void reset( void ) = 0;

  public:

  NetPipe writeBuffer;
  NetPipe readBuffer;
};

/// @brief Interface to the client
///
/// This class's one job is to provide an interface to the client.
///
/// TODO - make it an action?
///
class NetInterface: public Command::Base {
  public:

  struct category : public beefocus_tag {};
  using char_type = char;

  NetInterface()
  {
  }
  virtual ~NetInterface()
  {
  }

  virtual NetConnection& get() = 0;

  virtual Time::TimeUS execute() override final 
  {
    return get().execute();
  }

  virtual std::unique_ptr<NetConnection> connect( const std::string& location, unsigned int port ) = 0;

  private:
};


#endif

