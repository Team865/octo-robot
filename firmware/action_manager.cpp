#include "action_manager.h"

namespace Command {

Scheduler::Scheduler(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg ) :
    net{ netArg },
    hardware{ hardwareArg },
    debug{ debugArg },
    timeInUs{ 0 }
{
}

void Scheduler::addCommand( std::shared_ptr< Command::Base > interface )
{
  (*net) << "Command " << interface->debugName() << " added\n";
  size_t slot = actions.size();
  actions.push_back( interface );
  nextActionQueue.push( PriorityAndActionSlot( timeInUs, ActionSlotIndex( slot )));
}

//
// 1. Pop the next command to be executed from the priority queue
// 2. Update manager time (caller was responsible for doing the actual delay )
// 3. Run the Action
// 4. Figure out the next time the command should be run
// 5. Add the command back into the queue, at the new time
// 6. Figure out when the next command will be run & return the value
//
Time::TimeUS Scheduler::execute() 
{
  // 1. Pop the next action to be executed from the priority queue
  PriorityAndActionSlot current = nextActionQueue.top();
  nextActionQueue.pop();

  // 2. Update manager time (caller was responsible for doing the actual delay )
  timeInUs = current.first;

  // 3. Run the Action
  ActionSlotIndex index = current.second;
  Time::TimeUS actionDelayRequestUs = actions.at( index.get() )->execute();

  // 4. Figure out the next time the action should be run
  Time::DeviceTimeUS rescheduleAt = timeInUs + actionDelayRequestUs;

  // 5. Add the action back into the queue, at the new time
  nextActionQueue.push( PriorityAndActionSlot( rescheduleAt, current.second ));

  // 6. Figure out when the next action will be run & return the value
  Time::TimeUS delay_to_next_action(nextActionQueue.top().first - timeInUs );
  return delay_to_next_action;
}

} // end Action namespace


