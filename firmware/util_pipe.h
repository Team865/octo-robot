#ifndef __UTIL_PIPE__
#define __UTIL_PIPE__

#include <assert.h>

namespace Util {

/// @brief A classic pipe
///
/// CharType - the character type.  Probably best to use char.
/// size     - The size of the pipe.  Must be a power of 2
/// 
template< class CharType, size_t size >
class Pipe
{
  public:

  // Power of 2 check

  static_assert( (size | ( size-1 )) == ( 2 * size - 1), "size is not a power of 2" );
  static_assert( size > 0, "size has to be greater than 0"); 
  static constexpr size_t indexMask = size-1;

  using Buffer = std::pair< CharType*, size_t >;

  /// @brief Constructor
  ///
  /// @param[in] PusherArg:  A function that gets called when the pipe is
  ///     full.  In the case of a typical network the function would likely 
  ///     block while it waits for the network to clear data.  That's bad.
  ///
  Pipe( std::function<void(Pipe&)> pusherArg) :
    pusher{ pusherArg },
    writeIndex{ 0 },
    readIndex{ 0 }
  {
  }

  Pipe() = delete;

  ///
  /// @brief Write a single character into the pipe
  /// 
  /// Calls the push function if the pipe is full, which is expected
  /// to be blocking, and probably bad.
  ///
  void putChar( CharType c ) {
    m_buffer[ writeIndex ] = c;
    const size_t nextWriteBuffer = ( writeIndex + 1 ) & indexMask;
    if ( readIndex == nextWriteBuffer ) 
    { 
      pusher( *this );
      assert( readIndex != nextWriteBuffer ); 
    }
    writeIndex = nextWriteBuffer;
  }

  ///
  /// @brief Get a character from the pipe
  /// 
  /// Returns "0" if there's nothing to be read
  ///
  CharType getChar() {
    if ( readIndex == writeIndex ) { return 0; }
    CharType rval = m_buffer[readIndex];
    readIndex = ( readIndex + 1 ) & indexMask;
    return rval;
  }

  /// @brief Get a continous buffer for reading from
  ///
  /// @param[in] maxSize :  The maximum size of the buffer
  ///
  /// The buffer is valid until the next call to the pipe.  The intent here
  /// is to support a model where a non-blocking IO handler can write
  /// a certain number of bytes (maxSize) to some downstream service without
  /// blocking. 
  /// 
  Buffer readView( size_t maxSize ) {
    // 1.  Early exit if there's data to be read
    //
    if ( readIndex == writeIndex ) { return { nullptr, 0 }; };
  
    // 2. Figure out where the read index will be after the read
    //
    //    a. Start by assuming we get everything requested
    size_t nextReadIndex = readIndex + maxSize;
    //    b. Don't go beyound the end of the pipe buffer
    nextReadIndex = std::min( nextReadIndex, size );
    //    c. If the write index is ahead of us, don't go beyound that 
    if ( writeIndex > readIndex ) {
      nextReadIndex = std::min( nextReadIndex, writeIndex ); 
    }

    // 3. Construct the Buffer
    //
    return Buffer{ &(m_buffer[readIndex]), nextReadIndex - readIndex };
  }
  
  void readAdvance( std::size_t numToAdvance ) 
  {
    readIndex = (readIndex + numToAdvance ) & indexMask;
  }
  

  /// @brief Get a continous buffer for writing to
  ///
  /// @param[in] maxSize :  The maximum size of the buffer
  ///
  /// The buffer is valid until the next call to the pipe.  The intent here
  /// is to support a model where a non-blocking IO handler can read
  /// a certain number of bytes (maxSize) to some downstream service without
  /// blocking. 
  /// 
  Buffer writeView( size_t maxSize ) 
  {
    // 1.  Early exit if Buffer is full.
    //
    // We don't push in this case, the assumption is that the actor on the
    // other end is writing bytes from the network.
    //
    size_t maxWriteIndex = ( readIndex + size - 1 ) & indexMask;
    if ( writeIndex == maxWriteIndex )
    { 
      return { nullptr, 0 }; 
    };

    // 2. Figure out where the read index will be after the read
    //
    //    a. Start by assuming we get everything requested
    size_t nextWriteIndex = writeIndex + maxSize;
    //    b. Don't go beyound the end of the pipe buffer
    nextWriteIndex = std::min( nextWriteIndex , size );
    //    c. Don't crash into the read buffer
    if ( maxWriteIndex > writeIndex ) {
      nextWriteIndex = std::min( nextWriteIndex, maxWriteIndex ); 
    }
  
    return Buffer{ &(m_buffer[writeIndex]), nextWriteIndex - writeIndex};
  }
  
  void writeAdvance( std::size_t numToAdvance ) 
  {
    writeIndex = (writeIndex + numToAdvance ) & indexMask;
  }


private:
  std::function<void(Pipe&)> pusher;
  // index to the next character that will be written
  size_t writeIndex;
  // index to the next character that will be read.
  // does not point to a valid character if readIndex = writeIndex;
  size_t readIndex;
  std::array< CharType, size > m_buffer;
};
}; // end namespace Util

#endif

