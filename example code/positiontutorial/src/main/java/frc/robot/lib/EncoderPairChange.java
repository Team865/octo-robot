/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.lib;
 
 /**
  * Used to compute the difference between a Differential Drive's current & last encoder state
  *
  * Answers the question:  how much did the two robot wheels move since the last time we checked?
  */
public class EncoderPairChange {
    private EncoderPairSource source;
    private EncoderPair lastPos;

    /**
     * Constructor.  Takes a source of encoder data as its input.
     * 
     * The differential drive is an example of an encoder input source.  It implements the
     * EncoderPairSource Interface, which allows this class to read data from it in an
     * abstract way.
     * 
     * @param sourceArg  A class that implements the EncoderPairSource interface
     */
    public EncoderPairChange( EncoderPairSource sourceArg )
    {
        source = sourceArg;
        lastPos = source.getEncoderPos();
    }

    /**
     * How much did the two drivetrain wheels move since the last time we checked?
     * 
     * @return  An EncoderPair that represents the distance the two wheels on the
     *          drivetrain moved (in meters) since the last time getChange() was called.
     */
    public EncoderPair getChange()
    {
        EncoderPair currentPos = source.getEncoderPos();
        EncoderPair diff = currentPos.minus( lastPos );
        lastPos = currentPos;
        if ( diff.getMag() > 10.0 ) {
            //
            // Hack alert.  The trainer robot doesn't have the true encoder state
            // at start up because the state is sent over the network, and the update
            // might be delayed.  We're quietly consuming large changes here and
            // pretending they never happened as a workaround.
            //
            // The fix is to tighten up the robot start-up semantics.
            //
            diff = new EncoderPair( 0.0, 0.0 );
        } 
        return diff;
    }
}
