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
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import frc.robot.lib.TestbotSpeedController;

/**
 * An example command that uses an example subsystem.
 */
public class TeleopCommand extends CommandBase {
  @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
  private XboxController driver = new XboxController(0);

  private TestbotSpeedController motorA;
  private TestbotSpeedController motorB;
  private DifferentialDrive drive;
  /**
   * Creates a new ExampleCommand.
   *
   * @param subsystem The subsystem used by this command.
   */
  public TeleopCommand() {
    motorA = new TestbotSpeedController("motora");
    motorB = new TestbotSpeedController("motorb");
    drive = new DifferentialDrive(motorA, motorB);
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    driver.collectControllerData();

    int nextMotorASpeed = 0;
    int nextMotorBSpeed = 0;
    int maxWheelSpeed = 100;
    int addedWheelSpeed = 100;

    if (driver.bButton.isHeldDown()) {
      nextMotorASpeed += addedWheelSpeed;
      nextMotorBSpeed += addedWheelSpeed;
    }
    if (driver.xButton.isHeldDown()){
      nextMotorASpeed += -1 * addedWheelSpeed;
      nextMotorBSpeed += -1 * addedWheelSpeed;
    }
    if (driver.leftBumper.isHeldDown()){
      nextMotorASpeed += addedWheelSpeed;
      nextMotorBSpeed += -1 * addedWheelSpeed;
    }
    if (driver.rightBumper.isHeldDown()){
      nextMotorASpeed += -1 * addedWheelSpeed;
      nextMotorBSpeed += addedWheelSpeed;
    }
    if (nextMotorASpeed > maxWheelSpeed){
      nextMotorASpeed = maxWheelSpeed;
    }
    else if (nextMotorASpeed < -1 * maxWheelSpeed){
      nextMotorASpeed = -1 * maxWheelSpeed;
    }

    if (nextMotorBSpeed > maxWheelSpeed){
      nextMotorBSpeed = maxWheelSpeed;
    }
    else if (nextMotorBSpeed < -1 * maxWheelSpeed){
      nextMotorBSpeed = -1 * maxWheelSpeed;
    }

    SmartDashboard.putString("Right Wheel", Integer.toString(nextMotorASpeed));
    SmartDashboard.putString("Left Wheel", Integer.toString(nextMotorBSpeed));

    drive.tankDrive(nextMotorASpeed, nextMotorBSpeed);
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
