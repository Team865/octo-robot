#include <numeric>
#include "util_profile.h"
#include "simple_ostream.h"

namespace Util {

Profile::Profile( const std::string& binNameArg ) 
  : binName{ binNameArg }
{
  std::fill( samples.begin(), samples.end(), 0 );
}

void Profile::reset()
{
  currentScale = 1;
  maxTime = Time::TimeUS{numBins};
  std::fill( samples.begin(), samples.end(), 0 );
}

void Profile::addSample( Time::TimeUS sample )
{
  while ( sample > maxTime ) {
    compress();
  }
  size_t slot = sample.get() / currentScale;
  samples.at( slot ) += 1;
}

void Profile::compress() 
{
  for ( size_t i = 0; i < numBins/2; ++i ) {
    samples.at( i ) = samples.at( i*2 ) + samples.at( i*2+1 );
  }
  for ( size_t i = numBins/2; i < numBins; ++i ) {
    samples.at( i ) = 0;
  }
  currentScale *= 2;
  maxTime = maxTime * 2;
}

void Profile::reportHistogram( NetInterface& net ) const
{
  net << "Histogram for " << binName << "\n";
  size_t max=-1;
  unsigned int num_samples=0;

  for ( size_t i = 0; i < numBins; ++i ) {
    if ( samples.at( i ) != 0 ) { max = i; };
    num_samples += samples.at( i );
  }

  if ( num_samples == 0 ) {
    net << "  No Samples\n";
    return;
  }

  const unsigned int skip=max/20+1;

  for ( unsigned int i = 0; i < 20; ++i ) {
    const size_t start = i * skip; 
    const size_t end   = start + skip - 1;
    unsigned samples_here = 0;

    for ( size_t index = start; index <= end; ++index ) {
      samples_here += samples.at( index );
    }

    const unsigned start_time = start * currentScale;
    const unsigned end_time   = end   * currentScale + currentScale-1;

    const unsigned percent = 100 * samples_here / num_samples; 

    if ( percent < 10 )  { net << " "; }
    if ( percent < 100 ) { net << " "; }
    net << percent << "  " << start_time << "uS " << " -> " << end_time << "uS\n";
  }
}

void Profile::reportOneLiner( NetInterface& net ) const
{
  net << binName;
  int padding = 20 - binName.length();
  if ( padding < 0 ) padding = 0;
  for ( int i = 0; i < padding; ++i )
  {
    net << " ";
  }
  net << " ";

  const unsigned numSamples = std::accumulate( samples.begin(), samples.end(), 0 );
  if ( numSamples == 0 ) { net << "No Samples\n"; return; }
 
  const unsigned sampleMedium = (numSamples * 50+49) / 100;
  const unsigned sample90p    = (numSamples * 90+89) / 100;
  const unsigned sample98p    = (numSamples * 98+97) / 100;
  const unsigned sample100p   = numSamples;

  auto timeBoundForSamples = [&]( unsigned int sample_target ) {
    unsigned int samples_seen = 0;

    for ( size_t i = 0; i < numBins; ++i ) {
      samples_seen += samples.at( i );
      if ( samples_seen >= sample_target ) {
        return Time::TimeUS( i * currentScale + currentScale-1 );
      }
    }
    return Time::TimeUS( numBins * currentScale );
  };

  net << "50% = " << timeBoundForSamples( sampleMedium ).get() << "uS   ";
  net << "90% = " << timeBoundForSamples( sample90p ).get() << "uS   ";
  net << "98% = " << timeBoundForSamples( sample98p ).get() << "uS   ";
  net << "max = " << timeBoundForSamples( sample100p ).get() << "uS\n";
}


} // end namespace util

