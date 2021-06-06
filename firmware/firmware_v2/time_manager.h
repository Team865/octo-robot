#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

#include <memory>
#include "time_interface.h"
#include "command_base.h"
#include "time_hst.h"

namespace Time {

class Manager: public Interface, public Command::Base {
  public:

  /// Query time every 8 hours.
  static constexpr unsigned int msBetweenTimeQueries = 1000 * 60 * 60 * 8;

  Manager( 
      std::shared_ptr< Time::Interface >  baseInterfaceArg,
      std::shared_ptr< Time::HST >        hstArg 
  )
    : baseInterface{ baseInterfaceArg },
      hst { hstArg }
  {
  }

  virtual Time::RealTimeS secondsSince1970() override final;
  virtual Time::TimeUS execute() override final;
  virtual const char* debugName() override final { return "Manager"; }

  private:

  std::shared_ptr< Time::Interface> baseInterface;
  bool timeQueried;

  DeviceTimeMS timeQueriedAt;
  RealTimeS queryTime;

  void baseInterfaceCheckForTimeSync( DeviceTimeMS msSinceDevStart );

  std::shared_ptr< Time::HST >        hst;
};

void intTimeToString( std::string& outString, unsigned int secondsSince1970 );
} // End Time Namespace

#endif

