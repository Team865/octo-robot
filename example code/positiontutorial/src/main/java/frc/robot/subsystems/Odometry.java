/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.subsystems;

import edu.wpi.first.wpilibj.geometry.*;
import frc.robot.lib.EncoderPair;
import frc.robot.lib.EncoderPairSource;
import frc.robot.lib.EncoderPairChange;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

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
public class Odometry extends SubsystemBase {
  // Used to figure out how much the Drive Train encoders changed since
  // the last time the Odometry system was updated.
  //
  // i.e., answers the question, "how much did the wheels move"
  private EncoderPairChange encoderChangeTracker;
  // Where are we from the start, in meters
  private Translation2d translation = new Translation2d();
  // How has our orientation changed since start
  private Rotation2d rotation = new Rotation2d();

  /**
   * Constructor
   * 
   * @param encoderSource  A source of Differential Drive encoder data (like the Drive sub-system)
   */
  public Odometry( EncoderPairSource encoderSource )
  {
    encoderChangeTracker = new EncoderPairChange( encoderSource );
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
    //
    // 1. Get the Difference in Encoder Positions (d) from the Differential Drive
    //
    final EncoderPair d = encoderChangeTracker.getChange();

    //
    // 2. Early exit if there's really no difference.  The robot is stopped
    //
    if ( d.getMag() < .01 ) { 
      return; 
    }

    //
    // 3. Update the translation & orientation based on how far the wheels moved.
    //
    // See https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-186-mobile-autonomous-systems-laboratory-january-iap-2005/study-materials/odomtutorial.pdf
    // for details about the math.
    //
    final double dBaseline = 17.0;   // Measured + experimental
    final double phi = (d.getRight() - d.getLeft() ) / dBaseline;
    final double dCenter = ( d.getLeft() + d.getRight() ) / 2.0;

    final Rotation2d deltaRotation = new Rotation2d( phi );
    final Translation2d deltaTranslation = new Translation2d( dCenter, 0.0 ).rotateBy( rotation );
    translation = translation.plus( deltaTranslation );
    rotation = rotation.plus( deltaRotation );

    return;
  }

  /**
   * Get the current Pose2d (translation + rotation) 
   * 
   * - Same basic interface as First's DifferentialDriveOdometry class
   * 
   * @return  The current position & orientation of the robot
   */
  public Pose2d getPoseMeters() {
    // Copy so we don't leak a reference to our internal data
    return new Pose2d( 
      new Translation2d( translation.getX() / 100.0, translation.getY() / 100.0 ),
      new Rotation2d( rotation.getRadians() ) );
  }
}
