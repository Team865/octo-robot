
#include <iostream>
#include <memory>
#include <unistd.h>
#include <time.h>
#include <math.h>   // for adding variation to simulated temperature.
#include <map>

#include "../firmware_v2/command_datasend.h"
#include "../firmware_v2/command_motor.h"
#include "../firmware_v2/command_process_input.h"
#include "../firmware_v2/command_scheduler.h"

#include "../firmware_v2/hardware_interface.h"
#include "../firmware_v2/time_interface.h"
#include "../firmware_v2/time_manager.h"
#include "../firmware_v2/time_hst.h"

std::shared_ptr<Command::Scheduler> scheduler;

class TimeInterfaceSim: public Time::Interface {
  public:
 
  Time::RealTimeS secondsSince1970() override {
    return Time::RealTimeS(time(nullptr));
  } 

};

class SimTimeHST: public Time::HST 
{
  public:
  SimTimeHST() 
  {
    startTime = Time::DeviceTimeMS(0);
    startTime = msSinceDeviceStart();    // side effects.
    startTimeUs = Time::DeviceTimeUS(0);
    startTimeUs = usSinceDeviceStart();    // side effects.
  }

  Time::DeviceTimeMS msSinceDeviceStart() override 
  {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );
    const unsigned int msFromS  = t.tv_sec * 1000;
    const unsigned int msFromNs = t.tv_nsec / 1000000;
    return Time::DeviceTimeMS( msFromS + msFromNs - startTime.get() ); 
  }

  Time::DeviceTimeUS usSinceDeviceStart() override 
  {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t );
    const unsigned long int usFromS  = t.tv_sec * 1000000;
    const unsigned long int usFromNs = t.tv_nsec / 1000;
    return Time::DeviceTimeUS( usFromS + usFromNs - startTimeUs.get() ); 
  }

  virtual Time::TimeUS execute() override final
  {
    return Time::TimeUS( 1000000 );
  }

  const char* debugName() override final {
    return "Sim High Speed Timer";
  }

  private:

  Time::DeviceTimeMS startTime;
  Time::DeviceTimeUS startTimeUs;
};

class NetConnectionSim: public NetConnection {
  public:

  operator bool(void) {
    return true;
  }

  void reset(void ) override
  {
  }

  void writePushImpl( NetPipe&) override {
    // should never happen
    assert(0);
  }

  Time::TimeUS execute() override {
    while ( char c = writeBuffer.getChar() ) {
      putchar(c);
    }

    fd_set readfds;
    FD_ZERO(&readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    FD_SET(STDIN_FILENO, &readfds );
    while ( select(1, &readfds, nullptr, nullptr, &timeout ))
    {
      std::string input;
      std::cin >> input;
      for ( auto& charIn : input ) {
        readBuffer.putChar( charIn );
      }
      readBuffer.putChar( '\n' );
    }

    return Time::TimeUS( 50 );
  }
};

class NetInterfaceSim: public NetInterface {
  public:

  struct category: virtual beefocus_tag {};
  using char_type = char;

  NetInterfaceSim( std::shared_ptr<DebugInterface> debugLog )
  {
    (*debugLog) << "Simulator Net Interface Init\n";
  }

  const char* debugName() override { return "NetInterfaceSim"; }

  Time::TimeUS execute() override 
  {
    defaultConnection.execute();
    return Time::TimeUS{ 20000 }; // 1/50th of a second
  }
  std::shared_ptr<NetConnection> getShared()
  {
    return std::shared_ptr<NetConnection>();
  }

  NetConnection& get() { return defaultConnection; }

  NetConnectionSim defaultConnection;
};

namespace HW {
class ISim: public I
{
  public: 

  void PinMode( Pin pin, PinIOMode mode ) override
  {
    std::cout << "PM (" << HW::pinNames.at(pin) << ") = " << HW::pinIOModeNames.at(mode) << "\n";
  }
  void DigitalWrite( Pin pin, PinState state ) override
  {
    const std::string name = HW::pinNames.at(pin);
    std::cout << "DW (" << HW::pinNames.at(pin) 
              << ") = " << HW::pinStateNames.at( state ) 
              << "\n";
  }
  PinState DigitalRead( Pin pin ) override
  {
    return HW::PinState::INPUT_LOW;
  }
  unsigned AnalogRead( Pin pin ) override
  {
    static unsigned int count = 0;
    int count_pos = ((count / 2 ) & 0xfff )/256;   // Range 0 - 0xff
    int count_amp = (count & 1) ? count_pos : -count_pos ;
    count++;

    return 200 + count_amp;
  }

  IEvent& GetInputEvents( Pin pin ) override
  {
    return pinToEventMap[ pin ];
  }

  void LEDSet( unsigned int led, unsigned char r, unsigned char g, unsigned char b ) override 
  {
  }

  void LEDUpdate() override
  {
  }

  void beginTransmission( int address ) override
  {
  }

  void endTransmission() override
  {
  }
  
  void write( std::basic_string_view< std::byte > bytes ) override 
  {
  }

  void WireBeginTransmission( int address ) override
  {
  }

  bool WireEndTransmission() override
  {
    return (false);
  }

  void WireWrite(int data) override
  {
  }

  int WireRead() override
  {
    return(0);
  }

  int WireAvailable() override
  {
    return(0);
  }

  int WireRequestFrom(int address, int quantity) override
  {
    return(0);
  }

  private:

  std::map< Pin, IEvent > pinToEventMap;
};
}; // end HW namespace

class DebugInterfaceSim: public DebugInterface
{
  struct category: virtual beefocus_tag {};
  using char_type = char;

  std::streamsize write( const char_type* s, std::streamsize n ) override
  {
    // Ignore for now.
    return n;
  }
  void disable() override 
  {
    // Can't disable what we're ignoring.
  }
};

Time::TimeUS loop() {
  return scheduler->execute();
}

void setup() {
  auto debug      = std::make_shared<DebugInterfaceSim>();
  auto wifi       = std::make_shared<NetInterfaceSim>( debug );
  auto hardware   = std::make_shared<HW::ISim>();
  auto hst        = std::make_shared<SimTimeHST>();

  scheduler = std::make_shared<Command::Scheduler>( 
                          wifi, hardware, debug, hst );

  auto timeSim    = std::make_shared<TimeInterfaceSim>();
  auto time       = std::make_shared<Time::Manager>( timeSim, hst );
  auto motorSimA  = std::make_shared<Command::Motor>( hardware, debug );
  auto motorSimB  = std::make_shared<Command::Motor>( hardware, debug );
  auto encoderASim = std::make_shared<Command::Encoder>(
                          hardware, debug, wifi);
  auto encoderBSim = std::make_shared<Command::Encoder>(
                          hardware, debug, wifi);
 
  auto sr04        = std::make_shared<Command::SR04> (
                          hardware, debug, wifi, hst,
                          HW::Pin::SR04_TRIG, HW::Pin::SR04_ECHO );

  auto dataSend = std::make_shared<Command::DataSend>( 
                          debug, wifi, 
                          encoderASim, encoderBSim, sr04, hardware );

  auto commandProcessor= std::make_shared<Command::ProcessCommand>( 
                          wifi, hardware, debug, 
                          time, 
                          motorSimA,    motorSimB, 
                          encoderASim,  encoderBSim,
                          sr04,
                          hst,
                          scheduler,
                          dataSend
   );

  scheduler->addCommand( commandProcessor );
  scheduler->addCommand( time );
  scheduler->addCommand( hst );
  scheduler->addCommand( motorSimA );
  scheduler->addCommand( motorSimB );
  scheduler->addCommand( sr04 );
  scheduler->addCommand( encoderASim );
  scheduler->addCommand( encoderBSim );
  scheduler->addCommand( wifi );
  scheduler->addCommand( dataSend );
}

int main(int argc, char* argv[])
{
  setup();
  for ( ;; ) 
  {
    Time::TimeUS delay = loop();
    usleep( delay.get() );
  }
  return 0;
}

