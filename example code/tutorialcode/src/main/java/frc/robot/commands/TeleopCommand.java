/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import frc.robot.lib.XboxController;
import frc.robot.subsystems.OctoDriveSubsystem;
import frc.robot.subsystems.OctoDriveSubsystem.motorState;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.CommandBase;

/**
 * An example command that uses an example subsystem.
 */
public class TeleopCommand extends CommandBase {
  @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
  private XboxController driverController = new XboxController(0);
  private OctoDriveSubsystem octoDrive;
  /**
   * Creates a new ExampleCommand.
   *
   * @param subsystem The subsystem used by this command.
   **/

  public TeleopCommand(OctoDriveSubsystem initSubsystem) {
    octoDrive = initSubsystem;
    // Use addRequirements() here to declare subsystem dependencies.
    addRequirements(initSubsystem);
  }
  
  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    driverController.controllerPeriodic();

    SmartDashboard.putString("A Button", driverController.getIsAButtonPressed() + "");
    SmartDashboard.putString("B Button", driverController.getIsBButtonPressed() + "");
    SmartDashboard.putString("X Button", driverController.getIsXButtonPressed() + "");
    SmartDashboard.putString("Y Button", driverController.getIsYButtonPressed() + "");

    SmartDashboard.putString("L Bumper", driverController.getIsLBumperPressed() + "");
    SmartDashboard.putString("R Bumper", driverController.getIsRBumperPressed() + "");

    int rightMotorDirection = 0;
    int leftMotorDirection = 0;
    motorState rightMotorState = motorState.STOPPED;
    motorState leftMotorState = motorState.STOPPED;

    if(driverController.getIsBButtonPressed()){
      rightMotorDirection++;
      leftMotorDirection++;
    }
    if(driverController.getIsAButtonPressed()){
      rightMotorDirection--;
      leftMotorDirection--;
    }
    if(driverController.getIsLBumperPressed()){
      rightMotorDirection--;
      leftMotorDirection++;
    }
    if(driverController.getIsRBumperPressed()){
      rightMotorDirection++;
      leftMotorDirection--;
    }

    
    if(rightMotorDirection > 0){
      rightMotorState = motorState.FORWARD;
    }
    else if(rightMotorDirection < 0){
      rightMotorState = motorState.BACKWARDS;
    }


    if(leftMotorDirection > 0){
      leftMotorState = motorState.FORWARD;
    }
    else if(leftMotorDirection < 0){
      leftMotorState = motorState.BACKWARDS;
    }

    octoDrive.setMotors(rightMotorState, leftMotorState);
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
    octoDrive.setMotors(motorState.STOPPED, motorState.STOPPED);
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }


}
