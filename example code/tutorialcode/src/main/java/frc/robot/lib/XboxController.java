package frc.robot.lib;

import edu.wpi.first.hal.FRCNetComm;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DriverStation;

public class XboxController {
    
    private boolean isAButtonPressed = false;
    private boolean isBButtonPressed = false;
    private boolean isXButtonPressed = false;
    private boolean isYButtonPressed = false;
    private boolean isLBumperPressed = false;
    private boolean isRBumperPressed = false;

    private DriverStation driverStation = DriverStation.getInstance();
    private int port;

    public XboxController(int init_port){
        port = init_port;
        HAL.report(FRCNetComm.tResourceType.kResourceType_XboxController, port + 1);
    }

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