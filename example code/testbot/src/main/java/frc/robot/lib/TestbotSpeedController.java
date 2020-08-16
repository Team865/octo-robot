package frc.robot.lib;


import edu.wpi.first.wpilibj.SpeedController;

public class TestbotSpeedController implements SpeedController {
    private double motorSpeed;
    private boolean inverted;
    private String name;

    public TestbotSpeedController(String setName){
        motorSpeed = 0.0;
        inverted = false;
        name = setName;
    }

    @Override
    public void disable(){
        motorSpeed = 0.0;
        update();
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
        motorSpeed = speed * 100;
        update();
    }

    @Override
    public void setInverted(boolean isInverted){
        inverted = isInverted;
        update();
    }

    @Override
    public void stopMotor(){
        motorSpeed = 0.0;
        update();
    }

    @Override
    public void pidWrite(double output) {
    }

    private void update(){
        InternetConnecter internet = InternetConnecter.getInstance();
        if(!inverted){
            internet.sendData(name + "=" + motorSpeed);
        }
        else{
            internet.sendData(name + "=" + motorSpeed * -1.0);
        }
        internet.getData();
    }
}