#include "action_manager.h"

ActionManager::ActionManager(
    std::shared_ptr<NetInterface> netArg,
    std::shared_ptr<HWI> hardwareArg,
    std::shared_ptr<DebugInterface> debugArg ) :
    net{ netArg },
    hardware{ hardwareArg },
    debug{ debugArg },
    timeInUs{ 0 }
{
}

void ActionManager::addAction( std::shared_ptr< ActionInterface > interface )
{
  (*net) << "Action " << interface->debugName() << " added\n";
  size_t slot = interfaces.size();
  interfaces.push_back( interface );
  taskList.push( PriorityAndTaskSlot( timeInUs, slot ));
}

unsigned int ActionManager::loop() 
{
  PriorityAndTaskSlot current = taskList.top();
  taskList.pop();
  timeInUs = current.first;
  unsigned rescheduleAt = interfaces.at( current.second )->loop() + timeInUs;
  taskList.push( PriorityAndTaskSlot( rescheduleAt, current.second ));
  //(*net) << "Ran " << interfaces.at( current.second )->debugName() << " new time " << rescheduleAt << "\n"; 
  return taskList.top().first - timeInUs;
}

