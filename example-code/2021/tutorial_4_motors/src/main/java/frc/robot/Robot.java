// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import frc.robot.OctoSpeedController;

/**
 * This is a demo program showing the use of the RobotDrive class, specifically it contains the code
 * necessary to operate a robot with tank drive.
 */
public class Robot extends TimedRobot {
  private DifferentialDrive m_myRobot;
  private Joystick m_joyStick;

  
  @Override
  public void robotInit() {
    m_myRobot = new DifferentialDrive(new OctoSpeedController("motorr"), new OctoSpeedController("motorl"));
    m_joyStick = new Joystick(0);
  }

  @Override
  public void teleopPeriodic() {
    SmartDashboard.putString("Trigger",  m_joyStick.getTrigger() ? "Down" : "Up" );
    m_myRobot.tankDrive(-m_joyStick.getY(), -m_joyStick.getThrottle());
  }
}
