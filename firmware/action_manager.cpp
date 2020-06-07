#include "action_manager.h"

namespace Action {

Manager::Manager(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg ) :
    net{ netArg },
    hardware{ hardwareArg },
    debug{ debugArg },
    timeInUs{ 0 }
{
}

void Manager::addAction( std::shared_ptr< Interface > interface )
{
  (*net) << "Action " << interface->debugName() << " added\n";
  size_t slot = actions.size();
  actions.push_back( interface );
  nextActionQueue.push( PriorityAndActionSlot( timeInUs, slot ));
}

Time::TimeUS Manager::periodic() 
{
  PriorityAndActionSlot current = nextActionQueue.top();
  nextActionQueue.pop();
  timeInUs = current.first;
  Time::TimeUS actionDelayRequestUs = actions.at( current.second )->periodic();
  Time::DeviceTimeUS rescheduleAt = timeInUs + actionDelayRequestUs;
  nextActionQueue.push( PriorityAndActionSlot( rescheduleAt, current.second ));
  //(*net) << "Ran " << actions.at( current.second )->debugName() << " new time " << rescheduleAt << "\n"; 
  return Time::TimeUS (nextActionQueue.top().first - timeInUs );
}

} // end Action namespace


