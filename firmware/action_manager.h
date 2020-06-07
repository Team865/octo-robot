#ifndef __ACTION_MANAGER_H__
#define __ACTION_MANAGER_H__

#include <memory>     // for std::shared_ptr
#include <vector>     // for std::vector
#include <queue>      // for std::priority_queue

#include "action_interface.h"
#include "net_interface.h"
#include "debug_interface.h"
#include "hardware_interface.h"

namespace Action {

class Manager : public Interface {
  public:

  Manager(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg );

  void addAction( std::shared_ptr< Interface > interface );
  virtual unsigned int loop() override final;
  virtual const char* debugName() override { return "ActionManager"; }

  private:

  using PriorityAndTaskSlot = std::pair<unsigned, size_t >;

  std::shared_ptr<NetInterface> net;
  std::shared_ptr<HWI> hardware;
  std::shared_ptr<DebugInterface> debug;

  std::vector< std::shared_ptr< Interface >> interfaces;

  std::priority_queue< 
    PriorityAndTaskSlot, 
    std::vector<PriorityAndTaskSlot>, 
    std::greater<PriorityAndTaskSlot> > taskList;
  unsigned long long timeInUs;
};

} // end namespace Action

#endif

