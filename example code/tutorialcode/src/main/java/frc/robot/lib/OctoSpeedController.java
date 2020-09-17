package frc.robot.lib;


import edu.wpi.first.wpilibj.SpeedController;

/*
Please note that this is not an offical SpeedController, this is an inbetween
to work around the fact that the octorobot does not use a roboRIO. This
code is a way to fake the functionality of a normal SpeedController for
use in the OctoDriveSubsystem.

The OctoSpeedController is a custom SpeedController made to communicate with the
InternetConnecter. It represents and controlls the speed of one motor.
It is to be used as an input in a DifferentialDrive. It gives commands
to the InternetConnecter and the InternetConnecter sends them.
*/
public class OctoSpeedController implements SpeedController {
    private double motorSpeed;
    private boolean inverted;
    private String name;

    /*
    Init function. Store's the SpeedController's name and sets the
    motor's speed to 0.
    */
    public OctoSpeedController(String setName){
        motorSpeed = 0.0;
        inverted = false;
        name = setName;
    }

    /*
    Sets the speed of it's motor to 0.
    */
    @Override
    public void disable(){
        update(0.0);
    }

    /*
    Returns the speed of it's motor
    */
    @Override
    public double get(){
        return (motorSpeed);
    }

    /*
    Returns if the motor is invereted or not.
    */
    @Override
    public boolean getInverted(){
        return (inverted);
    }

    /*
    Sets the speed of the motor. Since the DifferentialDrive
    deals in speeds in the range of -1.0 to 1.0 we multiply the
    input by 100 as the octorobot deals in speeds of -100 to 100.
    */
    @Override
    public void set(double speed){
        update(speed * 100);
    }

    /*
    Sets the speed of the motor and updates the motor.
    */
    @Override
    public void setInverted(boolean isInverted){
        inverted = isInverted;
        update(motorSpeed);
    }

    /*
    Sets the speed of it's motor to 0.
    */
    @Override
    public void stopMotor(){
        update(0.0);
    }

    @Override
    public void pidWrite(double output) {
    }

    /*
    This function sends commands to the octorobot's pipe. It is called
    periodicly by the set function. If the speed has changed it updates
    the motor's speed.
    */
    private void update(double newSpeed){
        InternetConnecter internet = InternetConnecter.getInstance();
        if(newSpeed != motorSpeed){
            motorSpeed = newSpeed;
            //System.out.println(newSpeed);
            if(!inverted){
                internet.addToOutPipe(name + "=" + (int)motorSpeed);
            }
            else{
                internet.addToOutPipe(name + "=" + (int)(motorSpeed * -1.0));
            }
        }
        internet.periodic();
    }
}