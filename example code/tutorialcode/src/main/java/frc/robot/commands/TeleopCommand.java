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
  private XboxController driverController = new XboxController(0);

  /**
   * Creates a new ExampleCommand.
   *
   * @param subsystem The subsystem used by this command.
   
  public TeleopCommand() {
    m_subsystem = subsystem;
    // Use addRequirements() here to declare subsystem dependencies.
    addRequirements(subsystem);
  }
  */

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
