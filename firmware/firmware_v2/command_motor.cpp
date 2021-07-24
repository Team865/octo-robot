#include "command_motor.h"
#include "wifi_debug_ostream.h"

namespace Command{
//=======================================================================
Motor::Motor( 
  std::shared_ptr<HW::I> hwiArg,
  std::shared_ptr<DebugInterface> debugArg
) :
  dir { Motor::Dir::FORWARD }, speedAsPercent{ 0 }, counter{ 0 }
{
    int nDevices;
    int address;
    bool error;

    (*debugArg) << "Motor Up\n";
    (*debugArg) << "Scanning...\n";

    nDevices = 0;
    for(address = 1; address < 127; address++){
        hwiArg->WireBeginTransmission(address);
        error = hwiArg->WireEndTransmission();
        if(error){
            (*debugArg) << "I2C device found at address 0x";
            if(address < 16){
                (*debugArg) << "0";
            }
            (*debugArg) << address;
            (*debugArg) << "\n";
        }
    }


}

//
// Standard execute method
//
// Controls the speed of the motor by pulsing power to the motor.  If the
// motor is 50% on then execute generates a pulse that's "on" 50% of the
// time.  If the motor is 75% on, the pulse is "on" 75% of the time.
//
// See https://socratic.org/questions/how-do-you-calculate-the-period-and-frequency for terms
// 
// 1. Figure out if we're turning the motor on or off.
// 2. Figure out the pulse duration.  This is a function of speed
// 3. Early exit if we're spending no time in the current state
// 4. Set the hardware to the new state
// 5. Tell the schedule to pause, as per the pulse's duration
// 
Time::TimeUS Motor::execute() 
{

  return Time::TimeUS(100000);
}

//
// Set the current speed and direction
// 
void Motor::setSpeed( int percent )
{
  dir = percent >= 0 ? Dir::FORWARD : Dir::BACKWARDS;
  speedAsPercent = percent >= 0 ? percent : -percent;
}


//
// Get debug name
//  
const char* Motor::debugName()
{
  return "L298 Motor Control";
}


} // End Command Namespace


#ifdef TODO
#include "WEMOS_Motor.h"

int pwm;

//Motor shiled I2C Address: 0x30
//PWM frequency: 1000Hz(1kHz)
Motor M1(0x30,_MOTOR_A, 1000);//Motor A
Motor M2(0x30,_MOTOR_B, 1000);//Motor B


void setup2() {
  Serial.begin(115200);
}

void loop2() {

  for (pwm = 0; pwm <= 100; pwm++)
  {
    M1.setmotor( _CW, pwm);
    M2.setmotor(_CW, 100-pwm);
    Serial.printf("A:%d%, B:%d%, DIR:CW\r\n", pwm,100-pwm);
  }
  
  M1.setmotor(_STOP);
  M2.setmotor( _STOP);
  Serial.println("Motor A&B STOP");
  delay(200);
  
  for (pwm = 0; pwm <=100; pwm++)
  {
    M1.setmotor(_CCW, pwm);
    M2.setmotor(_CCW, 100-pwm);
    Serial.printf("A:%d%, B:%d%, DIR:CCW\r\n", pwm,100-pwm);

  }
  
  M1.setmotor(_STOP);
  M2.setmotor( _STOP);
  delay(200);
  Serial.println("Motor A&B STOP");

  M1.setmotor(_SHORT_BRAKE);
  M2.setmotor( _SHORT_BRAKE);
  Serial.println("Motor A&B SHORT BRAKE");  
  delay(1000);
  
  M1.setmotor(_STANDBY);//Both Motor standby
  //M2.setmotor( _STANDBY);
  Serial.println("Motor A&B STANDBY");  
  delay(1000);
  
}

#endif

