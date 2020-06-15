#ifndef __UTIL_PROFILE__
#define __UTIL_PROFILE__

#include <array>
#include "time_types.h"
#include "net_interface.h"

namespace Util {

// Internal profiling tool
//
class Profile
{
  public:

  static constexpr size_t numBins = 100;

  Profile( const std::string& binNameArg );
  Profile() = delete;

  void addSample( Time::TimeUS sample );
  void reportHistogram( NetInterface& net ) const;
  void reportOneLiner( NetInterface& net ) const;

  private:

  void compress();
  std::array< unsigned int, numBins > samples;
  const std::string binName;

  unsigned int currentScale = 1;
  Time::TimeUS maxTime = Time::TimeUS{numBins};
};

} // end Util namespace

#endif
