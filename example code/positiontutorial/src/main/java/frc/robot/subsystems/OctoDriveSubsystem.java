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
import frc.robot.lib.EncoderPair;
import frc.robot.lib.EncoderPairSource;
import frc.robot.lib.EncoderPairChange;
import edu.wpi.first.wpilibj.geometry.*;

public class OctoDriveSubsystem extends SubsystemBase implements EncoderPairSource {

  private double rightPower;
  private double leftPower;

  private OctoSpeedController rightController;
  private OctoSpeedController leftController;

  private OctoEncoder rightEncoder;
  private OctoEncoder leftEncoder;

  private DifferentialDrive drive;

  private Translation2d translation = new Translation2d();
  private Rotation2d rotation = new Rotation2d();

  EncoderPairChange encoderChangeTracker;

  /*
  OctoDriveSubsystem controls the movement of the wheels, it uses a WPIlib DifferentialDrive object. 
  Commands can use its functions to change the dirrection and speed of the wheels.
  */
  public OctoDriveSubsystem() {
    rightController = new OctoSpeedController("motorr");
    leftController = new OctoSpeedController("motorl");

    rightEncoder = new OctoEncoder("ENR");
    leftEncoder = new OctoEncoder("ENL");

    drive = new DifferentialDrive(rightController, leftController);

    leftEncoder.setDistancePerPulse(19.4/80.0 * 1.108);
    rightEncoder.setDistancePerPulse(19.4/80.0 * 1.108);

    leftEncoder.reset();
    rightEncoder.reset();    

    // Declare a class to track changes in the encoder, using this class as its data source
    encoderChangeTracker = new EncoderPairChange( this );
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
    doOdometryUpdate();
  
    drive.tankDrive(leftPower, rightPower);
  }

  /**
   * Quick and dirty map from a desired speed to a power setting
   * 
   * @param speed  The desired speed from -1 to 1.  -1 = full reverse, 1 = full forward
   * @return  The power setting that'll be sent to the motors
   */
  private static double speedToPower( double speed )
  {
    // The desired speed has a mangitude from 0 to 1.  After experimentation,
    // the motors start moving at power = .5 and, of course, reach full at 1.0.
    //
    // Well scale the desired speed mangitude of 0.0 to 1.0 to a power mangitude of
    // 0.5 to 1.0.
    //
    final double speedMagnitude = Math.abs( speed );
    final boolean isStopped = speedMagnitude == 0;
    final boolean isReversed = speed < 0;
    
    final double powerMagnitudeRaw = speedMagnitude * 0.5 + 0.5;
    final double powerMangitude = isStopped ? 0 : powerMagnitudeRaw;
    final double power = isReversed ? -powerMangitude : powerMangitude;

    return power;
  }

  /**
   * Changes the speed that the motors are going at.
   * <p>
   * Valid values for motor speeds are -1.0 to 1.0, with -1.0 representing
   * "full reverse" and 1.0 representing "full foirward".  A best effort is made
   * to map the desired speed into an actual speed on the robot (i.e., .5 should
   * have the robot go forward at about half the speed of 1.0).
   * 
   * @param newRightSpeed   The new speed of the right motor.
   * @param newLeftspeed    The new speed of the left mmotor
   * 
   */
  public void setMotors(double newRightSpeed, double newLeftSpeed){
    leftPower = speedToPower( newLeftSpeed );
    rightPower = speedToPower( newRightSpeed );
  }

  /**
   * Gets the current encoder position.
   * 
   * @return  An encoder pair that contains the total distance traveled by the left 
   *          and right encoders since robot start time
   */
  @Override
  public EncoderPair getEncoderPos()
  {
    double left = -leftEncoder.getDistance();
    double right = rightEncoder.getDistance();
    return new EncoderPair( left, right );
  }

  /**
   * Use left and right encoder data to estimate the robot's odometry
   */
  public void doOdometryUpdate(){

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