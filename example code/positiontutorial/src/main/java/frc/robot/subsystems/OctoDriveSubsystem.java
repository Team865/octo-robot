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

  //private boolean alignedDrive;

  private double x = 0.0;
  private double y = 0.0;
  private double angle = 0.0;

  private double lastLeftEncoder = 0;
  private double lastRightEncoder = 0;

  private boolean lastInitted = false;

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


      doAlignedDrive();
    

    drive.tankDrive(leftSpeed, rightSpeed);

  }

  /*
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

  static double computeAngleChange( double leftDelta, double rightDelta ) 
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

    double angleDelta; // change in angle

    final double leftSpeedMag = Math.abs( leftDelta );
    final double rightSpeedMag = Math.abs( rightDelta );

    if ( leftDelta == rightDelta ) {
      angleDelta = 0;
    }
    else {
      double leftRadius;
      double rightRadius;

      final boolean leftDeltaPos = leftDelta >= 0;
      final boolean rightDeltaPos = rightDelta >= 0;

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
      angleDelta = turnDirection * speedMagForAngleDelta / radiusForAngleDelta;
      //System.out.println("radius: " + radiusForAngleDelta + " speed: " + speedMagForAngleDelta + " angle delta: " + angleDelta );    
    }
    return angleDelta;
  }

  public void doAlignedDrive(){

    double leftDistance = leftEncoder.getDistance();
    double rightDistance = rightEncoder.getDistance();

    if ( !lastInitted ) {
      lastLeftEncoder = leftDistance;
      lastRightEncoder = rightDistance;
      lastInitted = true;
    }

    double leftDelta = -(leftDistance - lastLeftEncoder);
    double rightDelta = rightDistance - lastRightEncoder;

    lastLeftEncoder = leftDistance;
    lastRightEncoder = rightDistance;

    if ( Math.abs( leftDelta ) > 10.0 || Math.abs( rightDelta ) > 10.0 )
    {
      return;
    }

    final double leftSpeedMag = Math.abs( leftDelta );
    final double rightSpeedMag = Math.abs( rightDelta );

    if ( leftSpeedMag < .01 && rightSpeedMag < .01 ) {
      return;
    }

    double angleDelta = computeAngleChange( leftDelta, rightDelta );
 
    final double angleForPosCalc = angle + angleDelta / 2.0;
    // assume we move forward the average of the two wheels
    final double forward = ( leftDelta + rightDelta ) / 2.0;

    x += Math.cos( angleForPosCalc ) * forward;
    y += Math.sin( angleForPosCalc ) * forward;
    
    angle += angleDelta;
    //System.out.println( "lEncoder: " + leftDelta + " rEncoder: " + rightDelta + " angle: " + (((int) (angle * 180.0 / Math.PI)) + 360*100 ) % 360 + " adelta " + angleDelta );
    //System.out.println( "     " + "x= " + x + " y= " + y); 
    return;
  }

  public double getX() {
    return x;
  }
  public double getY() {
    return y;
  }

  public double getAngle() {
    return angle;
  }

}