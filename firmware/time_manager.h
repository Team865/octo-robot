#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

#include <memory>
#include "time_interface.h"
#include "action_interface.h"

namespace Time {

class Manager: public Interface, public Action::Interface {
  public:

  /// Query time every 8 hours.
  static constexpr unsigned int msBetweenTimeQueries = 1000 * 60 * 60 * 8;

  Manager( std::shared_ptr< Time::Interface > baseInterfaceArg )
    : baseInterface{ baseInterfaceArg }
  {
  }

  virtual unsigned int secondsSince1970() override final;
  virtual Time::DeviceTimeMS msSinceDeviceStart() override final;
  virtual unsigned int loop() override final;
  virtual const char* debugName() override final { return "Manager"; }

  private:

  std::shared_ptr< Time::Interface> baseInterface;
  bool timeQueried;

  DeviceTimeMS timeQueriedAt;
  unsigned int queryTime;

  void baseInterfaceCheckForTimeSync( DeviceTimeMS msSinceDevStart );

};

void intTimeToString( std::string& outString, unsigned int secondsSince1970 );
} // End Time Namespace

#endif

