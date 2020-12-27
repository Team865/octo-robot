/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

 package frc.robot.lib;
 
 /**
  * Implements the concept of a pair of encoders found a Differential Drive.
  *
  * Mostly used by the odometry subsystem
  */
public class EncoderPair {

  private double left;
  private double right;

  public EncoderPair( double leftArg, double rightArg )
  {
    left = leftArg;
    right = rightArg;
  }

  /**
    * Subtracts "other" from the current encoder and returns the result
    * 
    * @param other  What we want to subtract.  Result is this - other.
    * @return       The difference between the current encoder and other.
    */
  public EncoderPair minus( EncoderPair other ) {
    return new EncoderPair( left - other.left, right - other.right );
  }

  /**
    * Returns the "magnitude" of the current encoder pair.  Can be used as a metric
    * for "how far has the the drivetrain moved".
    * 
    * @return  The sum of the magnitude of the left & right encoder distance.
    */
  public double getMag() {
    return Math.abs( left ) + Math.abs( right );
  }

  /**
    * Convert the encoder state to a string.  Useful for debugging.
    * 
    * @return  A human readable string representing the encoder's state
    */
  public String toString()
  {
    return "EncoderP{ " + left + " , " + right + " }";
  }

  /**
   * Get the left encoder's position in Meters
   * 
   * @return Left encoder's position in Meters
   */
  public double getLeft() 
  {
    return left;
  }

  
  /**
   * Get the right encoder's position in Meters
   * 
   * @return right encoder's position in Meters
   */
  public double getRight()
  {
    return right;
  }
};
