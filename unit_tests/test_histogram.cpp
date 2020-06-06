#include <gtest/gtest.h>

#include "histogram.h"

/// @brief Unsigned integers 
TEST( HISTOGRAM, should_work_for_uints )
{
  using TestH = Histogram< unsigned int, 10>;
  TestH h( 100, 200 );

  h.insert( 0 );
  h.insert( 150 );
  h.insert( 250 );

  TestH::array_t golden = { 33, 0, 0, 0, 0, 33, 0, 0, 0, 34 };
  TestH::array_t actual;
  h.get_histogram( actual ); 
  ASSERT_EQ( golden, actual );
}

/// @brief Unsigned integers 
TEST( HISTOGRAM, should_work_for_uints_2 )
{
  using TestH = Histogram< unsigned int, 10>;
  TestH h( 100, 200 );

  h.insert( 150 );
  h.insert( 149 );
  h.insert( 189 );
  h.insert( 190 );

  TestH::array_t golden = { 0, 0, 0, 0, 25, 25, 0, 0, 25, 25 };
  TestH::array_t actual;
  h.get_histogram( actual ); 
  ASSERT_EQ( golden, actual );
}

/// @brief Low Resolution
TEST( HISTOGRAM, low_resolution )
{
  using TestH = Histogram< unsigned int, 10>;
  TestH h( 0, 19 );

  for ( int i = 0; i <= 19; ++i )
  {
    h.insert( i );
  }

  TestH::array_t golden = { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };
  TestH::array_t actual;
  h.get_histogram( actual ); 
  ASSERT_EQ( golden, actual );
}

/// @brief float test
TEST( HISTOGRAM, should_work_for_floats )
{
  using TestH = Histogram< float, 10 >;
  TestH h( 100.0f, 200.0f );

  h.insert( 0.0f );
  h.insert( 150.0f );
  h.insert( 250.0f );

  TestH::array_t golden = { 33, 0, 0, 0, 0, 33, 0, 0, 0, 34 };
  TestH::array_t actual;
  h.get_histogram( actual ); 
  ASSERT_EQ( golden, actual );
}

/// @brief Second float test
TEST( HISTOGRAM, should_work_for_floats_2 )
{
  using TestH = Histogram< float, 10 >;
  TestH h( 100.0f, 200.0f );

  h.insert( 150.0f );
  h.insert( 149.0f );
  h.insert( 189.0f );
  h.insert( 190.0f );

  TestH::array_t golden = { 0, 0, 0, 0, 25, 25, 0, 0, 25, 25 };
  TestH::array_t actual;
  h.get_histogram( actual ); 
  ASSERT_EQ( golden, actual );
}


