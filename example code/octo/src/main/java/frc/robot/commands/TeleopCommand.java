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
import frc.robot.lib.InternetConnecter;
import frc.robot.lib.OctoSpeedController;

/**
 * An example command that uses an example subsystem.
 */
public class TeleopCommand extends CommandBase {
  @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
  private XboxController driver = new XboxController(0);

  private OctoSpeedController motorA;
  private OctoSpeedController motorB;
  private int motorASpeed;
  private int motorBSpeed;
  private DifferentialDrive drive;
  /**
   * Creates a new ExampleCommand.
   *
   * @param subsystem The subsystem used by this command.
   */
  public TeleopCommand() {
    motorA = new OctoSpeedController("motora");
    motorB = new OctoSpeedController("motorb");
    drive = new DifferentialDrive(motorA, motorB);
    motorASpeed = 0;
    motorBSpeed = 0;
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

    InternetConnecter internet = InternetConnecter.getInstance();

    if (driver.bButton.isHeldDown()) {
      nextMotorASpeed += addedWheelSpeed;
      nextMotorBSpeed += addedWheelSpeed;
    }
    if (driver.xButton.isHeldDown()){
      nextMotorASpeed -= addedWheelSpeed;
      nextMotorBSpeed -= addedWheelSpeed;
    }
    if (driver.leftBumper.isHeldDown()){
      nextMotorASpeed += addedWheelSpeed;
      nextMotorBSpeed -= addedWheelSpeed;
    }
    if (driver.rightBumper.isHeldDown()){
      nextMotorASpeed -= addedWheelSpeed;
      nextMotorBSpeed += addedWheelSpeed;
    }

    nextMotorASpeed = Math.min(nextMotorASpeed, maxWheelSpeed);
    nextMotorASpeed = Math.max(nextMotorASpeed, -maxWheelSpeed);

    nextMotorBSpeed = Math.min(nextMotorBSpeed, maxWheelSpeed);
    nextMotorBSpeed = Math.max(nextMotorBSpeed, -maxWheelSpeed);

    motorASpeed = nextMotorASpeed;
    motorBSpeed = nextMotorBSpeed;

    SmartDashboard.putString("Right Wheel", Integer.toString(motorASpeed));
    SmartDashboard.putString("Left Wheel", Integer.toString(motorBSpeed));

    drive.tankDrive(motorASpeed, motorBSpeed);
    internet.periodic();
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
    drive.tankDrive(0, 0);
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }
}
