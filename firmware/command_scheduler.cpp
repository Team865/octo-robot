#include "command_scheduler.h"

namespace Command {

Scheduler::Scheduler(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<Time::HST> hstArg ) :
    net{ netArg },
    hardware{ hardwareArg },
    debug{ debugArg },
    hst{ hstArg },
    timeInUs{ 0 }
{
}

void Scheduler::addCommand( std::shared_ptr< Command::Base > interface )
{
  (*net) << "Command " << interface->debugName() << " added\n";
  size_t slot = actions.size();
  actions.push_back( ActionRecord{ interface, { interface->debugName() } } );
  nextCommandQueue.push( PriorityAndCommandSlot( timeInUs, CommandSlotIndex( slot )));
}

//
// 1. Pop the next command to be executed from the priority queue
// 2. Update manager time (caller was responsible for doing the actual delay )
// 3. Run the Command
// 4. Figure out the next time the command should be run
// 5. Add the command back into the queue, at the new time
// 6. Figure out when the next command will be run & return the value
//
Time::TimeUS Scheduler::execute() 
{
  // 1. Start the clock
  Time::DeviceTimeUS startTime = hst->usSinceDeviceStart();

  // 2. Pop the next action to be executed from the priority queue
  PriorityAndCommandSlot current = nextCommandQueue.top();
  nextCommandQueue.pop();

  // 2. Grather informatin
  const CommandSlotIndex index = current.second;
  ActionRecord& action = actions.at( index.get() );
  std::shared_ptr<Base>& command = action.first;
  Util::Profile& profile = action.second;

  // 2. Update the anager time (caller was responsible for the actual delay)
  timeInUs = current.first;

  // 4. Run the Command
  Time::TimeUS actionDelayRequestUs = command->execute();

  // 4. Figure out the next time the action should be run
  Time::DeviceTimeUS rescheduleAt = timeInUs + actionDelayRequestUs;

  // 5. Add the action back into the queue, at the new time
  nextCommandQueue.push( PriorityAndCommandSlot( rescheduleAt, index ));

  // 6. Figure out when the next action will be run & return the value
  Time::TimeUS delay_to_next_action(nextCommandQueue.top().first - timeInUs );

  // 1. Record profile data
  Time::DeviceTimeUS endTime = hst->usSinceDeviceStart();
  profile.addSample( Time::TimeUS(endTime - startTime )); 

  return delay_to_next_action;
}

void Scheduler::dumpProfile() const
{
  for ( const auto& action: actions ) 
  {
    const Util::Profile& profile = action.second;
    profile.reportOneLiner( *net );
  }
}


} // end Command namespace


