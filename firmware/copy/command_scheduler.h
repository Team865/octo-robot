#ifndef __COMMAND_MANAGER_H__
#define __COMMAND_MANAGER_H__

#include <memory>     // for std::shared_ptr
#include <vector>     // for std::vector
#include <queue>      // for std::priority_queue

#include "command_base.h"
#include "debug_interface.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "time_interface.h"
#include "util_profile.h"
#include "time_hst.h"

namespace Command {

// TODO - document

class Scheduler: Base {
  public:

  using ActionRecord = std::pair< std::shared_ptr< Base >, Util::Profile >;

  Scheduler(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HW::I> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<Time::HST> hstArg );

  void addCommand( std::shared_ptr< Base > interface );
  virtual Time::TimeUS execute() override final;
  virtual const char* debugName() override { return "CommandScheduler"; }

  void dumpProfile() const;
  void resetProfile();
  void scheduleProfile();

  private:

  struct CommandSlotIndexTag {};
  using CommandSlotIndex = UrbanRobot::TypeSafeNumber< size_t, CommandSlotIndexTag >;


  using PriorityAndCommandSlot = std::pair<Time::DeviceTimeUS, CommandSlotIndex >;

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HW::I> hardware;
  std::shared_ptr<DebugInterface> debug;
  std::shared_ptr<Time::HST > hst;

  std::vector< ActionRecord > actions;

  //
  // keep the actions in "next action to run" order.  probably safe from
  // memory fragmentation - the std::priority_queue is a container wrapper
  // that probably just maintains a heap on the vector.  The max size of
  // the vector won't change during runtime.
  //
  std::priority_queue< 
    PriorityAndCommandSlot, 
    std::vector<PriorityAndCommandSlot>, 
    std::greater<PriorityAndCommandSlot> > nextCommandQueue;
  Time::DeviceTimeUS timeInUs;
  bool profileScheduled;
};

} // end namespace Command

#endif

