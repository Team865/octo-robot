#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

#include <memory>
#include "time_interface.h"
#include "action_interface.h"

class TimeManager: public TimeInterface, public ActionInterface {
  public:

  /// Query time every 8 hours.
  static constexpr unsigned int msBetweenTimeQueries = 1000 * 60 * 60 * 8;

  TimeManager( std::shared_ptr< TimeInterface > baseInterfaceArg )
    : baseInterface{ baseInterfaceArg },
      timeQueried{ false }
  {
  }

  virtual unsigned int secondsSince1970() override final;
  virtual unsigned int msSinceDeviceStart() override final;
  virtual unsigned int loop() override final;
  virtual const char* debugName() override final { return "TimeManager"; }

  private:

  std::shared_ptr<TimeInterface> baseInterface;
  bool timeQueried;

  unsigned int timeQueriedAt;
  unsigned int queryTime;

  void baseInterfaceCheckForTimeSync( unsigned int msSinceDevStart );

};

void intTimeToString( std::string& outString, unsigned int secondsSince1970 );

#endif

