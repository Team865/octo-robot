#ifndef __ACTION_INTERFACE_H__
#define __ACTION_INTERFACE_H__

namespace Action {

class Interface {
  public:

  virtual unsigned int loop() = 0;
  virtual const char* debugName() = 0;

};

} // end namespace Action

#endif

