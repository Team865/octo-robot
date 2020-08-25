#include "command_datasend.h"
#include "wifi_debug_ostream.h"

namespace Command{

DataSend::DataSend( 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg, 
  std::shared_ptr<Command::Encoder> encoderLArg,
  std::shared_ptr<Command::Encoder> encoderRArg,
  std::shared_ptr<Command::SR04> rangeFinderArg
) :
  debug { debugArg }, net { netArg }, 
  encoderL{ encoderLArg },
  encoderR{ encoderRArg },
  rangeFinder{ rangeFinderArg },
  isOutputting{ false }
{
}

//
// Standard execute method
//
Time::TimeUS DataSend::execute() 
{
  if ( isOutputting ) {
    net->get() << "ENL " << encoderL->getPosition() << " " << encoderL->getSpeed() << "\n";  
    net->get() << "ENR " << encoderR->getPosition() << " " << encoderR->getSpeed() << "\n";
    rangeFinder->sensorRequest();  
  }
  return Time::TimeMS( 100 );  // 10 updates / second
}

//
// Get debug name
//  
const char* DataSend::debugName()
{
  return "Data Sender";
}

void DataSend::setOutput( bool isOutputtingArg )
{
  isOutputting = isOutputtingArg;
}

} // End Command Namespace

