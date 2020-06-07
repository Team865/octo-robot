#ifndef __ACTION_INTERFACE_H__
#define __ACTION_INTERFACE_H__

#include "time_interface.h"

namespace Action {

class Interface {
  public:

  virtual Time::TimeUS periodic() = 0;
  virtual const char* debugName() = 0;

};

} // end namespace Action

#endif

