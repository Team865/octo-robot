#ifndef __DATA_MOVER_H__
#define __DATA_MOVER_H__

#include <memory>
#include "action_interface.h"

#ifdef TODO
class TempInterface;
class NetInterface;

class DataMover: public ActionInterface {
  public:

  DataMover(
    std::string deviceNameArg, 
    std::shared_ptr<TempInterface> tempArg,
    std::shared_ptr<NetInterface> netArg
  );

  virtual unsigned int loop() override final;
  virtual const char* debugName() override final { return "DataMover"; }

  private:
 
  void packageData( const char* type, unsigned int data );
 
  std::shared_ptr<TempInterface> temp;
  std::shared_ptr<NetInterface> net;
  std::string deviceName;
};
#endif

#endif

