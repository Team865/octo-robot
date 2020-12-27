package frc.robot.lib;
 
 /**
  * Used to compute the difference between the Drive Train's current & last encoder state
  */
public class EncoderPairChange {
    private EncoderPairSource source;
    private EncoderPair lastPos;

    public EncoderPairChange( EncoderPairSource sourceArg )
    {
        source = sourceArg;
        lastPos = source.getEncoderPos();
    }

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
