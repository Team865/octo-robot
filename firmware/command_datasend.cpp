#include "command_datasend.h"
#include "wifi_debug_ostream.h"

namespace Command{

DataSend::DataSend( 
  std::shared_ptr<DebugInterface> debugArg, 
  std::shared_ptr<NetInterface> netArg, 
  std::shared_ptr<Command::Encoder> encoderAArg,
  std::shared_ptr<Command::Encoder> encoderBArg
) :
  debug { debugArg }, net { netArg }, 
  encoderA{ encoderAArg },
  encoderB{ encoderBArg },
  isOutputting{ false }
{
}

//
// Standard execute method
//
Time::TimeUS DataSend::execute() 
{
  if ( isOutputting ) {
    net->get() << "ENL " << encoderA->getPosition() << " " << encoderA->getSpeed() << "\n";  
    net->get() << "ENR " << encoderB->getPosition() << " " << encoderB->getSpeed() << "\n";  
  }
  return Time::TimeMS( 20 );  // 50 updates / second
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

