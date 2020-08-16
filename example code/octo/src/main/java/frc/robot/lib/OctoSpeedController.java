package frc.robot.lib;


import edu.wpi.first.wpilibj.SpeedController;

public class OctoSpeedController implements SpeedController {
    private double motorSpeed;
    private boolean inverted;
    private String name;

    public OctoSpeedController(String setName){
        motorSpeed = 0.0;
        inverted = false;
        name = setName;
    }

    @Override
    public void disable(){
        update(0.0);
    }

    @Override
    public double get(){
        return (motorSpeed);
    }

    @Override
    public boolean getInverted(){
        return (inverted);
    }

    @Override
    public void set(double speed){
        update(speed * 100);
    }

    @Override
    public void setInverted(boolean isInverted){
        inverted = isInverted;
        update(motorSpeed);
    }

    @Override
    public void stopMotor(){
        update(0.0);
    }

    @Override
    public void pidWrite(double output) {
    }

    private void update(double newSpeed){
        InternetConnecter internet = InternetConnecter.getInstance();
        if(newSpeed != motorSpeed){
            motorSpeed = newSpeed;
            if(!inverted){
                internet.addToPipe(name + "=" + (int)motorSpeed);
            }
            else{
                internet.addToPipe(name + "=" + (int)(motorSpeed * -1.0));
            }
        }
    }
}