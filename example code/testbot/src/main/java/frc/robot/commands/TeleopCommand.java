/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import frc.robot.lib.XboxController;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.CommandBase;

/**
 * An example command that uses an example subsystem.
 */
public class TeleopCommand extends CommandBase {
  @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
  private XboxController driver = new XboxController(0);

  /**
   * Creates a new ExampleCommand.
   *
   * @param subsystem The subsystem used by this command.
   */
  public TeleopCommand() {
    // Use addRequirements() here to declare subsystem dependencies.
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    //System.out.println("Why does this always happen");
    driver.collectControllerData();

    int rightWheelSpeed = 0;
    int leftWheelSpeed = 0;
    int maxWheelSpeed = 100;
    int addedWheelSpeed = 100;
    //System.out.println("TeleopCommand is running");

    if (driver.bButton.isHeldDown()) {
      rightWheelSpeed += addedWheelSpeed;
      leftWheelSpeed += addedWheelSpeed;
      //SmartDashboard.putString("Right Wheel", "Forwards!");
      //SmartDashboard.putString("Left Wheel", "Forwards!");
      //SmartDashboard.putString("Robot State", "Going forwards.");
    }
    if (driver.xButton.isHeldDown()){
      rightWheelSpeed += -1 * addedWheelSpeed;
      leftWheelSpeed += -1 * addedWheelSpeed;
      //SmartDashboard.putString("Right Wheel", "Backwards!");
      //SmartDashboard.putString("Left Wheel", "Backwards!");
      //SmartDashboard.putString("Robot State", "Backing up.");
    }
    if (driver.leftBumper.isHeldDown()){
      rightWheelSpeed += addedWheelSpeed;
      leftWheelSpeed += -1 * addedWheelSpeed;
      //SmartDashboard.putString("Right Wheel", "Forwards!");
      //SmartDashboard.putString("Left Wheel", "Backwards!");
      //SmartDashboard.putString("Robot State", "Turning left.");
    }
    if (driver.rightBumper.isHeldDown()){
      rightWheelSpeed += -1 * addedWheelSpeed;
      leftWheelSpeed += addedWheelSpeed;
      //SmartDashboard.putString("Right Wheel", "Backwards!");
      //SmartDashboard.putString("Left Wheel", "Forwards!");
      //SmartDashboard.putString("Robot State", "Turning right.");
    }
    //else{
    //  SmartDashboard.putString("Right Wheel", "Stationary.");
    //  SmartDashboard.putString("Left Wheel", "Stationary.");
    //  SmartDashboard.putString("Robot State", "Staying still.");
    //}
    if (rightWheelSpeed > maxWheelSpeed){
      rightWheelSpeed = maxWheelSpeed;
    }
    else if (rightWheelSpeed < -1 * maxWheelSpeed){
      rightWheelSpeed = -1 * maxWheelSpeed;
    }

    if (leftWheelSpeed > maxWheelSpeed){
      leftWheelSpeed = maxWheelSpeed;
    }
    else if (leftWheelSpeed < -1 * maxWheelSpeed){
      leftWheelSpeed = -1 * maxWheelSpeed;
    }

    RobotCommand.nextASpeed = rightWheelSpeed;
    RobotCommand.nextBSpeed = leftWheelSpeed;
    SmartDashboard.putString("Right Wheel", Integer.toString(rightWheelSpeed));
    SmartDashboard.putString("Left Wheel", Integer.toString(leftWheelSpeed));
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }
}
