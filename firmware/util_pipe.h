#ifndef __UTIL_PIPE__
#define __UTIL_PIPE__

namespace util {

template< class CharType, size_t size >
class pipe
{
  pipe( std::function<void(Pipe&)> pusherArg) :
    pusher{ pusherArg },
    startBuffer{ m_buffer[0] },
    endBuffer{ m_buffer[ size-1 ] },
    writeBuffer{ startBuffer },
    readBuffer{ endBuffer }
  {
  }

  void putChar( CharType c ) {
    if ( writeBuffer == readBuffer ) {
      pusher( *this );
    }
    *writeBuffer=c;
    writeBuffer++;
    if ( writeBuffer == endBuffer ) { writeBuffer = startBuffer; }
  }

private:
  std::function<void(Pipe&)> pusher;
  CharType* startBuffer;
  CharType* endBuffer;
  CharType* writeBuffer;
  CharType* readBuffer;
  std::array< CharType, size > m_buffer;
}

#endif

