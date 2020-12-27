/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot;

import frc.robot.commands.AutonomousCommand;
import frc.robot.commands.TeleopCommand;
import frc.robot.subsystems.OctoDriveSubsystem;
import frc.robot.subsystems.Odometry;
import edu.wpi.first.wpilibj2.command.Command;

/**
 * This class is where the bulk of the robot should be declared.  Since Command-based is a
 * "declarative" paradigm, very little robot logic should actually be handled in the {@link Robot}
 * periodic methods (other than the scheduler calls).  Instead, the structure of the robot
 * (including subsystems, commands, and button mappings) should be declared here.
 */
public class RobotContainer {
  // The robot's subsystems and commands are defined here...
  
  private final OctoDriveSubsystem m_octoDriveSubsystem = new OctoDriveSubsystem();
  private final TeleopCommand m_teleopCommand = new TeleopCommand(m_octoDriveSubsystem);
  private final AutonomousCommand m_autonomousCommand = new AutonomousCommand(m_octoDriveSubsystem, new Odometry( m_octoDriveSubsystem ));

  /**
   * The container for the robot.  Contains subsystems, IO devices, and commands.
   */
  public RobotContainer() {
  }


  public Command getTeleopCommand() {
    return (m_teleopCommand);
  }

  public Command getAutonomousCommand(){
    return (m_autonomousCommand);
  }
}
