package frc.robot.lib;

import edu.wpi.first.hal.FRCNetComm;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DriverStation;

/*
The XboxController class has the role of taking in a port
and gathering controller input from the controller on said
port. Anybody who wishes to use the controller's input
can do so by calling one of the get methods of the class.
This class does not control the actions the robot takes when
diffrent buttons are pressed, that is the role of the commands
that use this class.
*/
public class XboxController {
    
    private boolean isAButtonPressed = false;
    private boolean isBButtonPressed = false;
    private boolean isXButtonPressed = false;
    private boolean isYButtonPressed = false;
    private boolean isLBumperPressed = false;
    private boolean isRBumperPressed = false;

    private DriverStation driverStation = DriverStation.getInstance();
    private int port;


    /*
    Init function.
    Stores the controller's port and reports the usage of a controller resorce.
    */
    public XboxController(int init_port){
        port = init_port;
        HAL.report(FRCNetComm.tResourceType.kResourceType_XboxController, port + 1);
    }


    /*
    The controller's periodic function. Note that this must be
    called by other classes and is not called automadicly.
    This function's role is to update what buttons are currently
    being pushed down and which aren't.
    */
    public void controllerPeriodic(){
        //This returns a binary number, one bit is given for each button
        int buttonValues = driverStation.getStickButtons(port);

        //If a button's allocated bit is equal to 1, it's being pressed.
        isAButtonPressed = (buttonValues & (1 << 0)) != 0;
        isBButtonPressed = (buttonValues & (1 << 1)) != 0;
        isXButtonPressed = (buttonValues & (1 << 2)) != 0;
        isYButtonPressed = (buttonValues & (1 << 3)) != 0;
        isLBumperPressed = (buttonValues & (1 << 4)) != 0;
        isRBumperPressed = (buttonValues & (1 << 5)) != 0;
    }



    /*
    Bunch of get functions to check to see if a button is
    being pushed down.
    */
    public boolean getIsAButtonPressed(){
        return(isAButtonPressed);
    }



    public boolean getIsBButtonPressed(){
        return(isBButtonPressed);
    }



    public boolean getIsXButtonPressed(){
        return(isXButtonPressed);
    }



    public boolean getIsYButtonPressed(){
        return(isYButtonPressed);
    }



    public boolean getIsLBumperPressed(){
        return(isLBumperPressed);
    }



    public boolean getIsRBumperPressed(){
        return(isRBumperPressed);
    }
}