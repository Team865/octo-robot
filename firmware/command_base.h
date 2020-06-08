#ifndef __COMMAND_INTERFACE_H__
#define __COMMAND_INTERFACE_H__

#include "time_interface.h"

namespace Command {

class Base {
  public:

  virtual Time::TimeUS execute() = 0;
  virtual const char* debugName() = 0;

};

} // end namespace Action

#endif

