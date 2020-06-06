#include "time_manager.h"

void TimeManager::baseInterfaceCheckForTimeSync( unsigned int msSinceDevStart )
{
  if ( timeQueried && ( msSinceDevStart - timeQueriedAt ) < msBetweenTimeQueries ) 
  {
    return;
  }
  timeQueried = true;
  queryTime = baseInterface->secondsSince1970();
  timeQueriedAt = msSinceDevStart;
}

unsigned int TimeManager::secondsSince1970()
{
  unsigned int msSinceDevStart = baseInterface->msSinceDeviceStart();
  baseInterfaceCheckForTimeSync( msSinceDevStart );

  return queryTime + ( msSinceDevStart - timeQueriedAt ) / 1000;
}

unsigned int TimeManager::msSinceDeviceStart()
{
  return baseInterface->msSinceDeviceStart();
}

unsigned int TimeManager::loop()
{
  unsigned int msSinceDevStart = baseInterface->msSinceDeviceStart();
  baseInterfaceCheckForTimeSync( msSinceDevStart );
  return 5000000;
}

void intTimeToString( std::string& outString, unsigned int secondsSince1970 )
{
  constexpr unsigned int secondsPerMinute = 60;
  constexpr unsigned int secondsPerHour = secondsPerMinute * 60;
  constexpr unsigned int secondsPerDay = secondsPerHour * 24;
  const unsigned int hours = ( secondsSince1970 % secondsPerDay ) / secondsPerHour;
  const unsigned int mins = ( secondsSince1970 % secondsPerHour ) / secondsPerMinute;
  const unsigned int secs = ( secondsSince1970 % secondsPerMinute );

  outString.clear();
  outString.push_back( '0' + hours / 10 );
  outString.push_back( '0' + hours % 10 );
  outString.push_back( ':' );
  outString.push_back( '0' + mins / 10 );
  outString.push_back( '0' + mins % 10 );
  outString.push_back( ':' );
  outString.push_back( '0' + secs / 10 );
  outString.push_back( '0' + secs % 10 );
}



