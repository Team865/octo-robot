/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.subsystems;

import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.lib.OctoSpeedController;

public class OctoDriveSubsystem extends SubsystemBase {
  private motorState rightState;
  private motorState leftState;

  private OctoSpeedController rightController;
  private OctoSpeedController leftController;

  private DifferentialDrive drive;

  /**
   * Creates a new OctoDriveSubsystem.
   */
  public OctoDriveSubsystem() {
    rightState = motorState.STOPPED;
    leftState = motorState.STOPPED;

    rightController = new OctoSpeedController("motorr");
    leftController = new OctoSpeedController("motorl");

    drive = new DifferentialDrive(rightController, leftController);
  }

  @Override
  public void periodic() {
    double leftSpeed = convertStateToSpeed(leftState);
    double rightSpeed = convertStateToSpeed(rightState);

    drive.tankDrive(leftSpeed, rightSpeed);
  }

  private double convertStateToSpeed(motorState state){
    double returnSpeed = 0.0;
    if (state == motorState.FORWARD){
      returnSpeed = 1.0;
    }
    else if (state == motorState.BACKWARDS){
      returnSpeed = -1.0;
    }
    return (returnSpeed);
  }

  public void setMotors(motorState newRightState, motorState newLeftState){
    rightState = newRightState;
    leftState = newLeftState;
  }


  public enum motorState{
    FORWARD,
    STOPPED,
    BACKWARDS,
  }
}
