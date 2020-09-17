/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.subsystems;

import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.lib.OctoEncoder;
import frc.robot.lib.OctoSpeedController;

public class OctoDriveSubsystem extends SubsystemBase {
  private double rightSpeed;
  private double leftSpeed;

  private OctoSpeedController rightController;
  private OctoSpeedController leftController;

  private OctoEncoder rightEncoder;
  private OctoEncoder leftEncoder;

  private DifferentialDrive drive;

  private boolean alignedDrive;

  /*
  OctoDriveSubsystem controls the movement of the wheels, it uses a WPIlib DifferentialDrive object. 
  Commands can use its functions to change the dirrection and speed of the wheels.
  */
  public OctoDriveSubsystem() {
    rightController = new OctoSpeedController("motorr");
    leftController = new OctoSpeedController("motorl");

    rightEncoder = new OctoEncoder("ENR");
    leftEncoder = new OctoEncoder("ENL");

    rightSpeed = 0.0;
    leftSpeed = 0.0;

    alignedDrive = true;

    drive = new DifferentialDrive(rightController, leftController);
  }

  /*
  The periodic function constantly runs after the subsystem is created. It automaticly updates the speed
  of the drive's motors. Calling tankDrive periodicly is important as the DifferentialDrive is set
  to shut off if not updated enough as a safty feature.
  */
  @Override
  public void periodic() {
    rightEncoder.periodic();
    leftEncoder.periodic();

    if(alignedDrive){
      doAlignedDrive();
    }

    drive.tankDrive(leftSpeed, rightSpeed);

    //System.out.println(rightEncoder.getRaw());
    //System.out.println(leftEncoder.getRaw());
  }

  /*
  setMotors is called by commands and changes the speed that the motors are going at.
  These speeds will be sent to the DifferentialDrive next periodic command.
  */
  public void setMotors(double newRightSpeed, double newLeftSpeed){
    rightSpeed = newRightSpeed;
    leftSpeed = newLeftSpeed;
  }

  public void doAlignedDrive(){
  }

  public void setAlignedDrive(boolean newAlignedDrive){
    alignedDrive = newAlignedDrive;
  }
}