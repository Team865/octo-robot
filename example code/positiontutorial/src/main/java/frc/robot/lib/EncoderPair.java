 
 package frc.robot.lib;
 
 /**
  * Captures the concept of a left & right encoder pair
  */
public class EncoderPair {
    public EncoderPair( double leftArg, double rightArg )
    {
      left = leftArg;
      right = rightArg;
    }

    /**
     * Subtracts the other from the current encoder and returns the result
     * 
     * @param other  What we want to subtract.  Result is this - other.
     * @return       The difference betwene the current encoder and other.
     */
    public EncoderPair minus( EncoderPair other ) {
      return new EncoderPair( left - other.left, right - other.right );
    }

    /*
     * Returns the "magnitude" of the current encoder pair.  Can be used as a metric
     * for figuring out how far a pair of wheels moved.
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

    public double getLeft() 
    {
        return left;
    }

    public double getRight()
    {
        return right;
    }

    private double left;
    private double right;
  };
