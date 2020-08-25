#ifndef __COMMAND_SR04_H__
#define __COMMAND_SR04_H__

#include <memory>   // for std::shared_ptr
#include "command_base.h"
#include "hardware_interface.h"
#include "net_interface.h"
#include "debug_interface.h"
#include "time_hst.h"

namespace Command {

///
/// @brief SR04 Ultrasonic Sensor Command
///
/// Reference tutorial:  https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
///
/// Fast commands on the robot currently have a latency of around 10us.
/// The speed of sound is 340 m/s, or 0.034 cm/us.  It's an echo sensor,
/// so the sound has to go out and back.
///
/// If t is the time to get the response, in us, the range in cm is
/// 
/// range = t * 0.034 / 2
/// 
/// Reversing this...
///
/// range * 2 = t * 0.034
/// t = range * 2 / 0.034
///
/// The sensor has a range of 2cm to 100cm, so the expected wait time for a
/// response is between 117 us and 5882 us.
///
/// The upper end is a lot - we can't block while we're waiting for a response.
/// We'll schedule ourselves for a 20ms update while we wait for the sensor
/// to respond.  If we actually get called every 20ms the accuracy of the 
/// sensor will be about +.68 cm.  That might do.
///
/// If we do want more accuracy, another idea would be to do a course sensor
/// reading to get a general of the distance, and then do a fine sensor
/// reading that's a bit more blocking.  
/// 
class SR04: public Base {
  public:

  ///
  /// @brief Constructor
  ///
  /// @param[in] hwiArg     - Micro-controller Pin Interface
  /// @param[in] debugArg   - A debug console interface
  /// @param[in] netArg     - Interface to the WIFI network
  /// @param[in] pinTrigArg - "Trigger" output on the sensor
  /// @param[in] pinEchoArg - "Echo" input on the sensor
  /// 
  SR04( 
    std::shared_ptr<HW::I> hwiArg, 
    std::shared_ptr<DebugInterface> debugArg, 
    std::shared_ptr<NetInterface> netArg, 
    std::shared_ptr<Time::HST> hstArg,
    HW::Pin pinTrigArg,  
    HW::Pin pinEchoArg);

  ///
  /// @brief Standard time slice function
  /// @return The number of ms the scheduler should pause the command for
  ///         after execute runs
  ///
  virtual Time::TimeUS execute() override;

  ///
  /// @brief Standard "get debug name" function
  ///
  /// @return The debug name
  ///
  virtual const char* debugName() override;

  ///
  /// @brief Request a sensor reading.
  ///
  /// The result will be outputting to the net when the reading completes
  ///
  void sensorRequest();

  private:

  void handleAwaitingEcho();
  void handleSensorRequested();

  enum class Mode {
    IDLE,
    SENSOR_REQUESTED,
    AWAITING_ECHO
  };

  // @brief Interface to hardware (i.e., GPIO pins)
  std::shared_ptr<HW::I> hwi;
  // @brief Interface to debug log
  std::shared_ptr<DebugInterface> debug;
  // @brief Interface to network (i.e., Wifi)
  std::shared_ptr<NetInterface> net;
  // @brief Interface to the high speed timer
  std::shared_ptr<Time::HST > hst;

  // @brief Digital output to trigger a sound pulse
  const HW::Pin pinTrig;
  // @param Digital input to listen for the echo on
  const HW::Pin pinEcho;

  Mode mode;
};

}; // end Command namespace.

#endif

