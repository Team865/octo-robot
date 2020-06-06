#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include <algorithm>    // for std::min
#include <numeric>      // for std::accumulate
#include <iostream>     // for debugging.

///
/// @brief Histogram Template...
///
/// Probably over-kill, but I avoid using templates when I work in teams
/// and I need the practice.
/// 
/// T         The data-type we're sampling (i.e., floats, ints, most needs
///           a subtract, multiply, and a divide )
/// num_bins  The number of bins in the histogram
/// min       The minimum value
/// max       The maximum value.
///
template< class T, unsigned int num_bins >
class Histogram
{
  public:

  using array_t = std::array<unsigned int, (std::size_t) num_bins>;

  Histogram( T min_r, T max_r ) : min_range{ min_r }, max_range{ max_r }
  {
    reset();
  }

  void reset()
  {
    for ( auto& i : samples ) {
      i = 0U;
    }   
  }

  void insert( T sample )
  {
    const T range = max_range - min_range;
    const T from_min = sample - min_range;
    int bucket = ((int) (((T) num_bins ) * from_min )) / ((int) range);
    // bucket may be out of bounds, and that's okay.  clip.
    bucket = std::min( bucket, (int) (num_bins-1) );
    bucket = std::max( bucket, 0 );
    ++samples[ bucket ];
  }

  /// @brief takes the current samples and returns a histogram
  void get_histogram( array_t& histogram )
  {
    histogram = samples;
    unsigned int total_left = std::accumulate( histogram.begin(), histogram.end(), 0 );
    unsigned int histogram_left = 100;

    for ( auto& i : histogram )
    {
      if ( total_left == 0 ) { continue; }
      unsigned int this_histogram = (i * histogram_left) / total_left; 
      histogram_left -= this_histogram;
      total_left -= i;
      i = this_histogram;
    }
  }

  private:
  array_t samples;
  const T min_range;
  const T max_range;
};

#endif

