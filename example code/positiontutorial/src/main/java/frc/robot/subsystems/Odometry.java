package frc.robot.subsystems;

import edu.wpi.first.wpilibj.geometry.*;
import frc.robot.lib.EncoderPair;
import frc.robot.lib.EncoderPairSource;
import frc.robot.lib.EncoderPairChange;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class Odometry extends SubsystemBase {
  // Tracks how the encoder position changes
  private EncoderPairChange encoderChangeTracker;
  // Where are we from start, in meters
  private Translation2d translation = new Translation2d();
  // Orientation change from start
  private Rotation2d rotation = new Rotation2d();

  public Odometry( EncoderPairSource encoderSource )
  {
    encoderChangeTracker = new EncoderPairChange( encoderSource );
  }

  /**
   * Use left and right encoder data to estimate the robot's odometry
   */
  @Override
  public void periodic() {  

    final EncoderPair d = encoderChangeTracker.getChange();

    if ( d.getMag() < .01 ) { return; }

    // See https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-186-mobile-autonomous-systems-laboratory-january-iap-2005/study-materials/odomtutorial.pdf
    // for details about the math.

    final double dBaseline = 17.0;   // Measured + experimental
    final double phi = (d.getRight() - d.getLeft() ) / dBaseline;
    final double dCenter = ( d.getLeft() + d.getRight() ) / 2.0;

    final Rotation2d deltaRotation = new Rotation2d( phi );
    final Translation2d deltaTranslation = new Translation2d( dCenter, 0.0 ).rotateBy( rotation );
    translation = translation.plus( deltaTranslation );
    rotation = rotation.plus( deltaRotation );

    return;
  }

  public Pose2d getPoseMeters() {
    // "Clone" so we don't leak a reference to our internal data
    return new Pose2d( 
      new Translation2d( translation.getX() / 100.0, translation.getY() / 100.0 ),
      new Rotation2d( rotation.getRadians() ) );
  }
}
