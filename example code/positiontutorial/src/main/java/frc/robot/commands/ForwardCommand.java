package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.OctoDriveSubsystem;

public class ForwardCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    private OctoDriveSubsystem drive;
    private double xTarget;
    private double yTarget;
    boolean isFinishedFlag = false;

    public ForwardCommand(OctoDriveSubsystem driveArg, double xTargetArg, double yTargetArg) {
        drive = driveArg;
        xTarget = xTargetArg;
        yTarget = yTargetArg;
    }

    @Override
    public void initialize() {
        System.out.println("==== START FORWARD =====");
    }

    // Converts an angle from radians to degrees.
    //
    // TODO, put in common library
    //
    static int toDegrees( double radianAngle )
    {
        return ((int) (( radianAngle * 180.0 ) / Math.PI)) % 360;
    }

    // Computes angle x - y;
    //
    // TODO, put in common library
    //
    static int subtractDegrees( int x, int y ) {
        x = ( x + 360*100) % 360;               // Fix range from 0 - 359
        y = ( y + 360*100) % 360;               // Fix range from 0 - 359
        int diff = x - y;                       // Take the difference
        diff = ( diff + 360*100 ) % 360;            // Difference range 0 - 359
        diff = diff < 180 ? diff : diff - 360;  // If difference is above 180, negate it
        return diff;
    }

    @Override
    public void execute() {
        final double deltaX = xTarget - drive.getX();
        final double deltaY = yTarget - drive.getY();
        final double currentAngle = drive.getAngle();
        final double distanceLeft = Math.sqrt( deltaX * deltaX + deltaY * deltaY );

        final double targetAngle = Math.atan2( deltaY, deltaX );
        
        final int currentIntAngle = toDegrees( currentAngle );
        final int targetIntAngle = toDegrees( targetAngle );
        
        final int angleDiff = subtractDegrees( targetIntAngle, currentIntAngle );        

        //System.out.println( "Off target by " + diff + " distance " + distanceLeft + " last " + lastDistanceLeft );        

        //
        // Halt if we're close to the target
        //
        if ( distanceLeft < 10 ) {
            isFinishedFlag = true;
            drive.setMotors( 0.0, 0.0 );
            return;
        }
       
        // 
        // How big an angle change should we try to make.  Increases
        // as we get closer to the target (larger corrections allowed)
        // 
        final double closeGain = Math.max( 1.0, 3 - distanceLeft / 10 );

        // If the current angle differs from the the desired angle, make corrections
        // We'll always run one wheel at full, and slow down the opposite wheel a bit
        // to steer.
        //
        if ( angleDiff < 0 ) {
            double rightMtrPower = Math.max( 1.0 + ((double) angleDiff) * .05 * closeGain, .7 );
            //System.out.println("Adjustment " + smallMag );
            drive.setMotors( 1.0, rightMtrPower );
        }
        else {
            double leftMtrPower = Math.max( 1.0 - ((double) angleDiff) * .05 * closeGain, .7 );
            //System.out.println("Adjustment " + smallMag );            
            drive.setMotors( leftMtrPower, 1.0 );          
        }
    }

    @Override
    public void end(boolean interrupted) {
        drive.setMotors(0.0, 0.0);
    }

    @Override
    public boolean isFinished() {
        return isFinishedFlag;
    }
}