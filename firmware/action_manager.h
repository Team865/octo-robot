#ifndef __ACTION_MANAGER_H__
#define __ACTION_MANAGER_H__

#include <memory>     // for std::shared_ptr
#include <vector>     // for std::vector
#include <queue>      // for std::priority_queue

#include "action_interface.h"
#include "net_interface.h"
#include "debug_interface.h"
#include "hardware_interface.h"
#include "time_interface.h"

namespace Action {

class Manager : Interface {
  public:

  Manager(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg );

  void addAction( std::shared_ptr< Interface > interface );
  virtual Time::TimeUS periodic() override final;
  virtual const char* debugName() override { return "ActionManager"; }

  private:

  using PriorityAndActionSlot = std::pair<Time::DeviceTimeUS, size_t >;

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HWI> hardware;
  std::shared_ptr<DebugInterface> debug;

  std::vector< std::shared_ptr< Interface >> actions;

  //
  // keep the actions in "next action to run" order.  probably safe from
  // memory fragmentation - the std::priority_queue is a container wrapper
  // that probably just maintains a heap on the vector.  The max size of
  // the vector won't change during runtime.
  //
  std::priority_queue< 
    PriorityAndActionSlot, 
    std::vector<PriorityAndActionSlot>, 
    std::greater<PriorityAndActionSlot> > nextActionQueue;
  Time::DeviceTimeUS timeInUs;
};

} // end namespace Action

#endif

