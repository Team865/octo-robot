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
  private double rightSpeed;
  private double leftSpeed;

  private OctoSpeedController rightController;
  private OctoSpeedController leftController;

  private DifferentialDrive drive;



  /*
  OctoDriveSubsystem controls the movement of the wheels, it uses a WPIlib DifferentialDrive object. 
  Commands can use its functions to change the dirrection and speed of the wheels.
  */
  public OctoDriveSubsystem() {
    rightController = new OctoSpeedController("motorr");
    leftController = new OctoSpeedController("motorl");

    rightSpeed = 0.0;
    leftSpeed = 0.0;
    drive = new DifferentialDrive(rightController, leftController);
  }

  /*
  The periodic function constantly runs after the subsystem is created. It automaticly updates the speed
  of the drive's motors. Calling tankDrive periodicly is important as the DifferentialDrive is set
  to shut off if not updated enough as a safty feature.
  */
  @Override
  public void periodic() {
    drive.tankDrive(leftSpeed, rightSpeed);
  }

  /*
  setMotors is called by commands and changes the speed that the motors are going at.
  These speeds will be sent to the DifferentialDrive next periodic command.
  */
  public void setMotors(double newRightState, double newLeftState){
    rightSpeed = newRightState;
    leftSpeed = newLeftState;
  }
}