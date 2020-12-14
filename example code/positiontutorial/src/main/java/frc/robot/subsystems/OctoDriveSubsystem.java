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
      System.out.println("New Speed " + leftSpeed + "," + rightSpeed); 
    }
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

    // 17 cm gap between left and right, approx
    // 8.5cm radius from the center
    //
    // Let C be the center that the robot is pivoting around
    //
    // leftDelta * ( c - 8.5 ) = rightDelta * ( c + 8.5 )
    // leftDelta * c + leftdelta * 8.5 = rightDelta * c - rightDelta * 8.5
    // (leftDelta - rightDelta ) * c = -rightDelta * 8.5 - leftDelta * 8.5
    // c = ( rightDelta * 8.5 + leftDelta * 8.5 ) / ( leftDelta - rightDelta )

    if ( leftDelta == rightDelta ) {
      x += Math.cos( angle ) * leftDelta;
      y += Math.sin( angle ) * leftDelta;
    }
    else
    {
      double centerToWheel = 8.5;
      double c =   ( rightDelta * centerToWheel + leftDelta * centerToWheel ) /
                   ( leftDelta - rightDelta );
      // assume we move forward the average of the two wheels
      double forward = ( leftDelta + rightDelta ) / 2.0;

      System.out.println("MDeltas " + leftDelta +  " " + rightDelta + " c= " + c);

      double wheelPos;
      double wheelDistance;

      if ( Math.abs( leftDelta ) > Math.abs( rightDelta )) {
        wheelDistance = leftDelta;
        wheelPos = - centerToWheel;       
      }                  
      else {
        wheelDistance = rightDelta;
        wheelPos = centerToWheel;
      }
      x += Math.cos( angle ) * forward;
      y += Math.sin( angle ) * forward;

      double angleChange = wheelDistance / ( wheelPos - c );

      System.out.println( "C=" + c + " angleChange " + angleChange );
      angle += angleChange;
    }

    if ( leftDelta != 0.0 || rightDelta != 0.0 ) {
      System.out.println("Deltas " + leftDelta +  " " + rightDelta);
      System.out.println("X = " + x + " y = " + y + " angle = " + angle/2/3.141536*360.0 );
    }

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