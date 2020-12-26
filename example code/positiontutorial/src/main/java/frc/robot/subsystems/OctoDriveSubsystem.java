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

  /**
   * Captures the concept of a left & right encoder pair
   */
  class EncoderPair {
    EncoderPair( double leftArg, double rightArg )
    {
      left = leftArg;
      right = rightArg;
    }
    EncoderPair sub( EncoderPair rhs ) {
      return new EncoderPair( left - rhs.left, right - rhs.right );
    }

    double mag() {
      return Math.sqrt( left*left + right*right );
    }

    public String toString()
    {
      return "EncoderP{ " + left + " , " + right + " }";
    }

    public double left;
    public double right;
  };

  private double rightSpeed;
  private double leftSpeed;

  private OctoSpeedController rightController;
  private OctoSpeedController leftController;

  private OctoEncoder rightEncoder;
  private OctoEncoder leftEncoder;

  private DifferentialDrive drive;

  private double x = 0.0;
  private double y = 0.0;
  private double theta = 0.0;

  EncoderPair lastEncoderPos = null;

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

    drive = new DifferentialDrive(rightController, leftController);

    leftEncoder.setDistancePerPulse(19.4/80.0 * 1.108);
    rightEncoder.setDistancePerPulse(19.4/80.0 * 1.108);

    leftEncoder.reset();
    rightEncoder.reset();    
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
    

    drive.tankDrive(leftSpeed, rightSpeed);

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
  setMotors is called by commands and changes the speed that the motors are going at.
  These speeds will be sent to the DifferentialDrive next periodic command.
  */
  public void setMotors(double newRightSpeed, double newLeftSpeed){
    // Adjusting by ~50% and adding .5 gets us a sort of linear
    // input speed to actual speed curve.
    double adjustedRightSpeed = 0;
    if ( newRightSpeed > 0 ) {
      adjustedRightSpeed = newRightSpeed * .5 + .5;
    }
    if ( newRightSpeed < 0 ) {
      adjustedRightSpeed = newRightSpeed * .5 - .5;
    }
    double adjustedLeftSpeed = 0;
    if ( newLeftSpeed > 0 ) {
      adjustedLeftSpeed = newLeftSpeed * .5 + .5;
    }
    if ( newLeftSpeed < 0 ) {
      adjustedLeftSpeed = newLeftSpeed * .5 - .5;
    }

    if ( rightSpeed != adjustedRightSpeed || leftSpeed != adjustedLeftSpeed )
    {
      rightSpeed = adjustedRightSpeed;
      leftSpeed = adjustedLeftSpeed;
      //System.out.println("New Speed " + leftSpeed + "," + rightSpeed); 
    }
  }

  /**
   * Gets the current encoder position.
   * 
   * @return  An encoder pair that contains the total distance traveled by the left 
   *          and right encoders since robot start time
   */
  EncoderPair getEncoderPos()
  {
    double left = -leftEncoder.getDistance();
    double right = rightEncoder.getDistance();
    return new EncoderPair( left, right );
  }

  /**
   * Get the Delta (change) between the current encoder position & the last encoder position
   * 
   * Side effects:  Update's the class's last encoder position (lastPos)
   * 
   * @return An encoder pair that represents the delta between the left & right encoder positions
   */
  EncoderPair getDEncoder()
  {
    EncoderPair currentEncoderPos = getEncoderPos();
    if ( lastEncoderPos == null ) {
      lastEncoderPos = currentEncoderPos;
    }
    EncoderPair result = currentEncoderPos.sub( lastEncoderPos );
    lastEncoderPos = currentEncoderPos;
    return result;
  }

  static double computePhi( double dLeft, double dRight ) 
  {
    // 17 cm gap between left and right, approx
    // 8.5cm radius from the center
    //
    // Let centerToWheel = 8.5cm
    // let wheelGap = 17cm
    //
    // Basics:  If the wheel speed isn't equal the robot goes in a circle.  Define:
    //
    // leftRadius = the left wheel's circle radius
    // rightRadius = the right whee's circle radius
    // 
    // leftCircumfrance = leftRadius * 2 * Pi
    // rightCircumfrance = rightRadius * 2 * Pi
    //
    // leftCircumfrance / LeftSpeedMag = rightCircumfrance / rightSpeedMag
    // leftRadius * 2 * Pi / leftSpeedMag = rightRadius * 2 * Pi / rightSpeedMag
    // leftRadius / leftSpeedMag = rightRadius / rightSpeedMag
    // leftRadius * rightSpeedMag = rightRadius * leftSpeedMag    
    //
    // =============================================================
    //
    // Suppose the Right Wheel moves faster than the Left wheel
    // The robot turns in a center centered to the left of the left wheel
    // 
    // rightRadius = leftRadius + wheelGap (we pivot to the left of the left wheel)
    //
    // leftRadius * rightSpeedMag = rightRadius * leftSpeedMag       
    // leftRadius * rightSpeedMag = ( leftRadius + wheelGap ) * leftSpeedMag
    // leftRadius * rightSpeedMag = leftRadius * leftSpeedMag + wheelGap * leftSpeedMag
    // leftRadius * ( rightSpeedMag - LeftSpeedMag ) = wheelGap * leftSpeedMag
    // leftRadius = wheelGap * leftSpeedMag / (rightSpeedMag - LeftSpeedMag )
    // 
    // Suppose the Left Wheel moves faster than the Right wheel
    // The robot turns in a center centered to the right of the right wheel
    // 
    // rightRadius = leftRadius - wheelGap (we pivot to the right of the right wheel
    // 
    // leftRadius * rightSpeedMag = rightRadius * leftSpeedMag        
    // leftRadius * rightSpeedMag = ( leftRadius - wheelGap ) * leftSpeedMag
    // leftRadius * rightSpeedMag = leftRadius * leftSpeedMag - wheelGap * leftSpeedMag
    // leftRadius * ( rightSpeedMag - LeftSpeedMag ) = -wheelGap * leftSpeedMag
    // leftRadius = -wheelGap * leftSpeedMag / (rightSpeedMag - LeftSpeedMag ) 
    //
    // Suppose the left and right wheels move in opposite directions
    //
    // rightRadius = wheelGap - leftRadius
    // leftRadius * rightSpeedMag = rightRadius * leftSpeedMag        
    // leftRadius * rightSpeedMag = ( wheelGap - leftRadus ) * leftSpeedMag
    // leftRadius * rightSpeedMag = - leftRadius * leftSpeedMag + wheelGap * leftSpeedMag
    // leftRadius * ( rightSpeedMag + LeftSpeedMag ) = wheelGap * leftSpeedMag
    // leftRadius = wheelGap * leftSpeedMag / (rightSpeedMag + LeftSpeedMag ) 
    //
    // Suppose the left and right wheels move at the same speed...  We're going straight
    //
    // Computing angle (radians) change from radius and wheel distance traveled
    // 
    // circumfrance = radius * 2 * Pi
    // angle = ( wheelDistanceTraveled / circumfrance ) * M_PI * 2
    //       = wheelDistanceTraveled / (radius * M_PI * 2 ) * M_PI * 2
    //       = wheelDistanceTraveled / radius

    double phi; // change in angle

    final double leftSpeedMag = Math.abs( dLeft );
    final double rightSpeedMag = Math.abs( dRight );

    if ( dLeft == dRight ) {
      // Left and Right wheel are the same.  Forward.
      phi = 0;
    }
    else {
      double leftRadius;
      double rightRadius;

      final boolean leftDeltaPos = dLeft >= 0;
      final boolean rightDeltaPos = dRight >= 0;

      final double wheelGap = 16.5;
      float turnDirection;  // 1 = counterclockwise, -1 = clockwise

      if ( leftDeltaPos != rightDeltaPos ) { // sign mismatch
        leftRadius = wheelGap * leftSpeedMag / ( rightSpeedMag + leftSpeedMag );
        rightRadius = wheelGap - leftRadius;
        turnDirection = leftDeltaPos ? -1 : 1;
      }
      else if ( leftSpeedMag > rightSpeedMag ) {
        leftRadius = -wheelGap * leftSpeedMag / (rightSpeedMag - leftSpeedMag );
        rightRadius = leftRadius - wheelGap;
        turnDirection = -1;
      }
      else {
        leftRadius =  wheelGap * leftSpeedMag / (rightSpeedMag - leftSpeedMag );
        rightRadius = leftRadius + wheelGap;
        turnDirection = 1;        
      }
      double radiusForAngleDelta;
      double speedMagForAngleDelta;
      if ( leftRadius > rightRadius ) {
        radiusForAngleDelta = leftRadius;
        speedMagForAngleDelta = leftSpeedMag;
      }
      else {
        radiusForAngleDelta = rightRadius;
        speedMagForAngleDelta = rightSpeedMag;
      }
      phi = turnDirection * speedMagForAngleDelta / radiusForAngleDelta;
      //System.out.println("radius: " + radiusForAngleDelta + " speed: " + speedMagForAngleDelta + " angle delta: " + angleDelta );    
    }
    return phi;
  }

  /**
   * Use left and right encoder data to estimate the robot's odometry
   * 
   * See https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-186-mobile-autonomous-systems-laboratory-january-iap-2005/study-materials/odomtutorial.pdf
   * for details about the math.
   */
  public void doOdometryUpdate(){

    EncoderPair d = getDEncoder();

    //
    // Because we're not directly connected to the robot, there can be a
    // delay at start up between when the Java code comes up and when the
    // robot sends its first encoder update.  Quietly ignore any encoder
    // deltas that are ridiculously large.  TODO - should probably lock 
    // this down better.
    //
    if ( d.mag() > 10.0 ) { return; }
    if ( d.mag() < .01 ) { return; }

    double phi = computePhi( d.left, d.right );
 
    final double thetaForPosCalc = theta + phi / 2.0;
    final double dCenter = ( d.left + d.right ) / 2.0;

    x += Math.cos( thetaForPosCalc ) * dCenter;
    y += Math.sin( thetaForPosCalc ) * dCenter;
    theta += phi;

    return;
  }

  public double getX() {
    return x;
  }
  public double getY() {
    return y;
  }

  public double getTheta() {
    return theta;
  }

}