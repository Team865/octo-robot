/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.lib;

import edu.wpi.first.wpilibj.geometry.*;

/**
  * Like the First Robotics DifferentialDriveOdometry class, but doesn't need a hardware gyro
  *
  * Pros vs DifferentialDriveOdometry:  doesn't need a hardware gyro
  * Cons vs DifferentialDriveOdometry:  much less accurate
  *
  * Tries to follow DifferentialDriveOdometry interface when possible
  *
  * See https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-186-mobile-autonomous-systems-laboratory-january-iap-2005/study-materials/odomtutorial.pdf
  * for the math write-up
  */
public class DifferentialDriveOdometryNoGyro {

    // Where are we from the origin, in meters
    private Translation2d cTranslation;
    // How has our orientation changed since start
    private Rotation2d cRotation;
    // The distance between the two wheels
    private final double cDBaseLine;
    // The last left encoder position we saw
    private double cLastLeftDistanceMeters = 0.0;
    // The last right encoder position we saw
    private double cLastRightDistanceMeters = 0.0;
    
    /**
     * Constructs an object that estimates position & orientation given a differential drive
     * 
     * The initial translation of the odometry object is the origin;  <0,0,0>
     * The initial rotation of the odometry object is 0 degrees
     * 
     * @param dBaseLine             The distance between the wheels, in cm.
     */
    public DifferentialDriveOdometryNoGyro( double dBaseLine ) {
        cTranslation = new Translation2d();
        cRotation = new Rotation2d();
        cDBaseLine = dBaseLine;
    }

    /**
     * Constructs an object that estimates position & orientation given a differential drive
     * 
     * @param initialPoseMeters     The starting position of the robot on the field
     * @param dBaseLine             The distance between the wheels, in cm.
     */
    public DifferentialDriveOdometryNoGyro( Pose2d initialPoseMeters, double dBaseLine ) {
        final Translation2d translation = initialPoseMeters.getTranslation();
        cTranslation = new Translation2d( translation.getX(), translation.getY() );
        final Rotation2d rotation = initialPoseMeters.getRotation();
        cRotation = new Rotation2d( rotation.getRadians() );
        cDBaseLine = dBaseLine;
    }

    public Pose2d getPoseMeters()
    {
        return new Pose2d( 
            new Translation2d( cTranslation.getX(), cTranslation.getY() ),
            new Rotation2d( cRotation.getRadians() ) );
    }

    public Pose2d update( double leftDistanceMeters, double rightDistanceMeters )
    {
        double dLeft  = leftDistanceMeters - cLastLeftDistanceMeters;
        double dRight = rightDistanceMeters - cLastRightDistanceMeters;
        
        cLastLeftDistanceMeters  = leftDistanceMeters;
        cLastRightDistanceMeters = rightDistanceMeters;

        if ( dLeft + dRight > 0.1 ) {
            // Hack.  There can be a delay between when the Training Robot's internal
            // encoders are reset and the reporting because it's all done over ther
            // network.  For now, surpress any unrealisticly large encoder changes
            dLeft = 0.0;
            dRight = 0.0;
        }

  
        //
        // 3. Update the translation & orientation based on how far the wheels moved.
        //
        // See https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-186-mobile-autonomous-systems-laboratory-january-iap-2005/study-materials/odomtutorial.pdf
        // for details about the math.
        //
       
        final double phi = ( dRight - dLeft ) / cDBaseLine;
        final double dCenter = ( dRight + dLeft ) / 2.0;
  
        final Rotation2d deltaRotation = new Rotation2d( phi );
        final Translation2d deltaTranslation = new Translation2d( dCenter, 0.0 ).rotateBy( cRotation );
        cTranslation = cTranslation.plus( deltaTranslation );
        cRotation = cRotation.plus( deltaRotation );
  
        return getPoseMeters();
    }
  }
