#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <memory>   // for std::shared_ptr
#include "action_interface.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"

namespace Action {

class Motor: public ActionInterface {
  public:

  Motor( std::shared_ptr<HWI> hwiArg, std::shared_ptr<DebugInterface> debugArg, std::shared_ptr<NetInterface> netArg, HWI::Pin pin0,  HWI::Pin pin1);

  virtual unsigned int loop() override;
  virtual const char* debugName() override;

  void setSpeed( int percent );

  enum class Dir {
    FORWARD,
    BACKWARDS
  };

  private:

  std::shared_ptr<HWI> hwi;
  std::shared_ptr<DebugInterface> debug;
  std::shared_ptr<NetInterface> net;
  const HWI::Pin pin0;
  const HWI::Pin pin1;

  Dir dir;
  unsigned speed;
  int counter;
};

};

#endif

