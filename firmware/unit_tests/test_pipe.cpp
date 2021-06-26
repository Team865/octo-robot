#include <gtest/gtest.h>

#include "../firmware_v1/util_pipe.h"

namespace Util {

TEST( pipe_should, add_and_remove_single_characters )
{
  // using MyPipe = Pipe< char, 255 >;   Static Asserts, size != power of 2
  // using MyPipe = Pipe< char, 0 >;     Static Asserts, size has to be >0
  using MyPipe = Pipe< char, 64 >;
  unsigned pushCount=0;
  MyPipe pipe( [&pushCount]( MyPipe& pipe ) { pipe.getChar(); ++pushCount; });

  ASSERT_EQ( 0, pipe.getChar() );

  for ( char i = 0; i < 32; ++i ) {
    pipe.putChar( i );
  }
  for ( char golden = 0; golden < 32; ++golden ) {
    ASSERT_EQ( golden, pipe.getChar() );
  }
  ASSERT_EQ( 0, pushCount );
  ASSERT_EQ( 0, pipe.getChar() );
  for ( char i= 0; i< 64; ++i) {
    pipe.putChar( i );
  }
  // Should have overflowed the pipe once
  ASSERT_EQ( 1, pushCount );
  for ( char golden = 1; golden < 64; ++golden ) {
    ASSERT_EQ( golden, pipe.getChar() );
  }
}

TEST( pipe_should, do_read_buffers)
{
  using MyPipe = Pipe< char, 64 >;
  MyPipe pipe( []( MyPipe& pipe ) { assert(0); } );

  // Empty test
  MyPipe::Buffer rb0 = pipe.readView( 64 );
  ASSERT_EQ( nullptr, rb0.first );
  ASSERT_EQ( 0, rb0.second );

  // Simple test, first 32 characters
  for ( char i = 0; i < 32; ++i ) {
    pipe.putChar( i );
  }
  MyPipe::Buffer rb1 = pipe.readView( 64 );
  ASSERT_EQ( 32, rb1.second );
  for ( char golden = 0; golden < 32; ++golden ) {
    ASSERT_EQ( golden, rb1.first[ golden ] );
  }
  pipe.readAdvance( rb1.second );

  // Fill with 48 more characters for more tests.
  for ( char i = 0; i < 48; ++i ) {
    pipe.putChar( i );
  }

  // Small buffer test
  MyPipe::Buffer rb2 = pipe.readView( 10 );
  ASSERT_EQ( 10, rb2.second );
  for ( char golden = 0; golden < 10; ++golden ) {
    ASSERT_EQ( golden, rb2.first[ golden ] );
  }
  pipe.readAdvance( rb2.second );

  // Only go to end of pipe test
  MyPipe::Buffer rb3 = pipe.readView( 48 );
  ASSERT_EQ( 22, rb3.second );
  for ( char golden = 10; golden < 32; ++golden ) {
    ASSERT_EQ( golden, rb3.first[ golden - 10 ] );
  }
  pipe.readAdvance( rb3.second );
  
  // Grab first characters at the beginning of the pipe test
  MyPipe::Buffer rb4 = pipe.readView( 48 );
  ASSERT_EQ( 16, rb4.second );
  for ( char golden = 32; golden < 48; ++golden ) {
    ASSERT_EQ( golden, rb4.first[ golden - 32 ] );
  }
  pipe.readAdvance( rb4.second );

  // Should be empty again
  MyPipe::Buffer rb5 = pipe.readView( 48 );
  ASSERT_EQ( nullptr, rb5.first );
  ASSERT_EQ( 0, rb5.second );
}

TEST( pipe_should, do_write_buffers)
{
  using MyPipe = Pipe< char, 64 >;
  unsigned pushCount=0;
  MyPipe pipe( [&pushCount]( MyPipe& pipe ) { pipe.getChar(); ++pushCount; });

  // Request more than what's in the buffer, but don't advance
  MyPipe::Buffer rb0 = pipe.writeView( 96 );
  ASSERT_EQ( 63, rb0.second );

  // Request 32 bytes
  MyPipe::Buffer rb1 = pipe.writeView( 32 );
  ASSERT_EQ( 32, rb1.second );
  for ( char i = 0; i < 32; ++i ) {
    rb0.first[ i ] = i;
  }
  pipe.writeAdvance( 32 );

  for ( char golden = 0; golden < 32; ++golden ) {
    ASSERT_EQ( golden, pipe.getChar() );
  }
  // Request more than what's in the pipe.  Should get to the end of pipe
  MyPipe::Buffer rb2 = pipe.writeView( 96 );
  ASSERT_EQ( 32, rb2.second );
  for ( char i = 0; i < 32; ++i ) {
    rb2.first[ i ] = i;
  }
  pipe.writeAdvance( 32 );

  // Do another big request again.  Should go to the read buffer  
  MyPipe::Buffer rb3 = pipe.writeView( 96 );
  ASSERT_EQ( 31, rb3.second );
  for ( char i = 0; i < 30; ++i ) {
    rb3.first[ i ] = 32+i;
  }
  pipe.writeAdvance( 30 ); // only put in 30 characters, one short.

  // Should have no push events
  ASSERT_EQ( 0, pushCount );

  // We can add one more byte, and the buffer is full
  pipe.putChar( 30+32 ); 
  ASSERT_EQ( 0, pushCount );

  // Adding another byte to a full buffer should push
  pipe.putChar( 31+32 ); 
  ASSERT_EQ( 1, pushCount );

  // Check results
  for ( char golden = 1; golden < 64; ++golden ) {
    ASSERT_EQ( golden, pipe.getChar() );
  }
}

} // end namespace Util

