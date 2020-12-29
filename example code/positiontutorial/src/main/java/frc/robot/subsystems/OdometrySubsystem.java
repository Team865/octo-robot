/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.subsystems;

import edu.wpi.first.wpilibj.geometry.*;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.lib.DifferentialDriveOdometryNoGyro;

/**
 * Trainer robot Odometry Implementation...
 * 
 * This is simlar to the First Robots odometry class
 * https://docs.wpilib.org/en/stable/docs/software/kinematics-and-odometry/differential-drive-odometry.html
 * 
 * Except for the following:
 * 
 * - It's implemented as a Subsystem and reads Encoder Data directly from an EncoderPairSource interface on the drive
 * - The current training robot has no hardware gyroscope, so orientation is also computed using the encoder data
 * - Computing orientation this was is less accurating than using a gyroscope - that's why competition robots use gyroscopes.
 * 
 */
public class OdometrySubsystem extends SubsystemBase {
 
  // Object that does the odometry math
  DifferentialDriveOdometryNoGyro cOdometry;
  // Source of our encoder information
  OctoDriveSubsystem cDrive;

  /**
   * Constructor
   * 
   * @param encoderSource  A source of Differential Drive encoder data (like the Drive sub-system)
   */
  public OdometrySubsystem( OctoDriveSubsystem drive )
  {
    // 0.17m (17cm) is the approximate distance between our wheels
    cOdometry = new DifferentialDriveOdometryNoGyro( 0.17 );
    cDrive = drive;
  }

  /**
   * Uses left and right Differential Drive's encoder data to estimate the robot's odometry
   * 
   * 1. Get the Difference in Encoder Positions (d) from the Differential Drive
   * 2. Early exit if there's really no difference.  The robot is stopped
   * 3. Update the translation & orientation based on how far the wheels moved.
   */
  @Override
  public void periodic() { 
    cOdometry.update( cDrive.getLeftEncoder(), cDrive.getRightEncoder() );
  }

  /**
   * Get the current Pose2d (translation + rotation) 
   * 
   * - Same basic interface as First's DifferentialDriveOdometry class
   * 
   * @return  The current position & orientation of the robot
   */
  public Pose2d getPoseMeters() {
    return cOdometry.getPoseMeters();
  }
}
