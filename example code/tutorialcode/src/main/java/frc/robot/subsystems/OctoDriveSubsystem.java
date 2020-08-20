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



  /*
  This is the OctoDriveSubsystem class.
  It's role is to control the movement of the wheels, it uses a differential drive and an enum to store what it's
  two motors are currently doing. Commands can use it's functions to change the dirrection of the wheels.
  */
  public OctoDriveSubsystem() {
    rightState = motorState.STOPPED;
    leftState = motorState.STOPPED;

    rightController = new OctoSpeedController("motorr");
    leftController = new OctoSpeedController("motorl");

    drive = new DifferentialDrive(rightController, leftController);
  }



  /*
  The periodic function constantly runs after the subsystem is created. It automaticly updates the speed
  of the drive's motors. Calling tankDrive periodicly is important as the DifferentialDrive is set
  to shut off if not updated enough as a safty feature.
  */
  @Override
  public void periodic() {
    double leftSpeed = convertStateToSpeed(leftState);
    double rightSpeed = convertStateToSpeed(rightState);

    drive.tankDrive(leftSpeed, rightSpeed);
  }


  /*
  convertStateToSpeed takes in a motor's state and return the speed that it should be going at.
  Note that speed to the DifferentialDrive is mesured from -1.0 to 1.0.
  */
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


  /*
  setMotors is called by other commands and changes the speed that the motors are going at.
  These speeds will be sent to the DifferentialDrive next periodic command.
  */
  public void setMotors(motorState newRightState, motorState newLeftState){
    rightState = newRightState;
    leftState = newLeftState;
  }


  /*
  An Enum with the role of storing the 3 possable states of a motor, going forwards
  backwards or being stopped.
  */
  public enum motorState{
    FORWARD,
    STOPPED,
    BACKWARDS,
  }
}
