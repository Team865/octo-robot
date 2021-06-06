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

class NetConnection: public Command::Base {
  public:

  struct category : public beefocus_tag {};
  using char_type = char;

  /// Constructor 
  NetConnection() :
    writeBuffer{ [this]( NetPipe& pipe) { writePush(pipe);} },
    readBuffer { [this]( NetPipe& pipe) { readPush(pipe);} }
  {
  }

  /// Destructor
  virtual ~NetConnection()
  {
  }

  /// Called when we've filled the output pipe.  Forces a blocking write
  /// (i.e., clear the pipe, whatever it takes)
  void writePush(NetPipe& pipe)
  {
    writePushImpl( pipe );
  }

  /// Called when we've filled the input pipe.  Should never trigger unless
  /// there's a bug or malice.
  void readPush(NetPipe &)
  {
    assert(0);
  }


  ///
  /// @brief Legacy interface that transfers data from s into the write pipe.
  ///
  /// TODO,  remove and call the pipe directly for speed.
  ///
  std::streamsize write( const char_type* s, std::streamsize n )
  {
    for ( std::streamsize index = 0; index < n; ++index )
    {
      writeBuffer.putChar( s[index] );
    }
    return n;
  }

  /// 
  /// @brief Get a string from the read pipe.
  ///
  /// string[in,out]  The string...
  ///   input  - The string that's been assembled so far.
  ///   output - The string with the additions made this round 
  /// 
  /// @return true if there's a string to be consumed, false otherwise.
  ///
  /// Caller is expected to clear the string if we return true, and preserve
  /// it if we return false.
  ///
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

  /// 
  /// Called when the write FIFO is full.  Triggers a blocking write.
  ///
  virtual void writePushImpl( NetPipe& pipe ) = 0;

  ///
  /// Execute function called by the firmware scheduler.  This is where the
  /// pipes get cleared.  All functions here should be non-blocking.
  /// 
  virtual Time::TimeUS execute() = 0;

  const char* debugName() { return "NetConnection"; } 

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
  virtual std::shared_ptr<NetConnection> getShared() = 0;

  virtual Time::TimeUS execute()=0;

  //virtual std::unique_ptr<NetConnection> connect( const std::string& location, unsigned int port ) = 0;

  private:
};


#endif

