/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.lib;

/**
 * An abstract interace for something that's a source of Encoder Pair Data
 * 
 * For example, a Differential Drive has two encoders on each wheel.  This
 * interface lets a caller read that positions of those encoders in an abstract
 * way.
 */
public interface EncoderPairSource {
    /**
     * Get the current encoder position state.
     * 
     * @return  Returns the encoder position of the left & right wheels, in Meters.
     */
    public EncoderPair getEncoderPos();
}
