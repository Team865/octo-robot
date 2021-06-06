#include "command_scheduler.h"

namespace Command {

Scheduler::Scheduler(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HW::I> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg,
    std::shared_ptr<Time::HST> hstArg ) :
    net{ netArg },
    hardware{ hardwareArg },
    debug{ debugArg },
    hst{ hstArg },
    timeInUs{ 0 },
    profileScheduled{ false }
{
}

void Scheduler::addCommand( std::shared_ptr< Command::Base > interface )
{
  net->get() << "Command " << interface->debugName() << " added\n";
  size_t slot = actions.size();
  actions.push_back( ActionRecord{ interface, { interface->debugName() } } );
  nextCommandQueue.push( PriorityAndCommandSlot( timeInUs, CommandSlotIndex( slot )));
}

//
// 1. Start the clock
// 2. Pop the next command to be executed from the priority queue
// 3. Grather informatin
// 4. Update the manager time (caller was responsible for the actual delay)
// 5. Run the Command
// 6. Figure out the next time the action should be run
// 7. Add the action back into the queue, at the new time
// 8. Figure out when the next action will be run & return the value
// 9. Record profile data
//
Time::TimeUS Scheduler::execute() 
{
  // 1. Start the clock
  Time::DeviceTimeUS startTime = hst->usSinceDeviceStart();

  // 2. Pop the next command to be executed from the priority queue
  PriorityAndCommandSlot current = nextCommandQueue.top();
  nextCommandQueue.pop();

  // 3. Gather informatin
  const CommandSlotIndex index = current.second;
  ActionRecord& action = actions.at( index.get() );
  std::shared_ptr<Base>& command = action.first;
  Util::Profile& profile = action.second;

  // 4. Update the manager time (caller was responsible for the actual delay)
  timeInUs = current.first;

  // 5. Run the Command
  Time::TimeUS actionDelayRequestUs = command->execute();

  // 6. Figure out the next time the action should be run
  Time::DeviceTimeUS rescheduleAt = timeInUs + actionDelayRequestUs;

  // 7. Add the action back into the queue, at the new time
  nextCommandQueue.push( PriorityAndCommandSlot( rescheduleAt, index ));

  // 8. Figure out when the next action will be run & return the value
  Time::TimeUS delay_to_next_action(nextCommandQueue.top().first - timeInUs );

  // 9. Record profile data
  Time::DeviceTimeUS endTime = hst->usSinceDeviceStart();
  profile.addSample( Time::TimeUS(endTime - startTime )); 

  // Do profile dumps on the !tracked portion, so they don't pollute results
  if ( profileScheduled ) {
    dumpProfile();
    profileScheduled=false;
  }
  
  return delay_to_next_action;
}

void Scheduler::scheduleProfile()
{
  profileScheduled=true;
}

void Scheduler::dumpProfile() const
{
  for ( const auto& action: actions ) 
  {
    const Util::Profile& profile = action.second;
    profile.reportOneLiner( *net );
  }
}

void Scheduler::resetProfile()
{
  for ( auto& action: actions ) 
  {
    Util::Profile& profile = action.second;
    profile.reset();
  }
}


} // end Command namespace


