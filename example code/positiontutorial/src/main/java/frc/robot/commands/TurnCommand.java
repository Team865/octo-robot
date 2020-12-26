package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.OctoDriveSubsystem;

public class TurnCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    private OctoDriveSubsystem drive;
    private int targetIntAngle;
    private boolean atCorrectAngle;

    // Converts an angle from radians to degrees.
    //
    int toDegrees( double radianAngle )
    {
        return ((int) (( radianAngle * 180.0 ) / Math.PI)) % 360;
    }

    // Computes angle x - y;
    //
    int subtractDegrees( int x, int y ) {
        x = ( x + 360*100) % 360;               // Fix range from 0 - 359
        y = ( y + 360*100) % 360;               // Fix range from 0 - 359
        int diff = x - y;                       // Take the difference
        diff = ( diff + 360*100 ) % 360;            // Difference range 0 - 359
        diff = diff < 180 ? diff : diff - 360;  // If difference is above 180, negate it
        return diff;
    }

    public TurnCommand( OctoDriveSubsystem driveArg, double targetAngleArg) {
        drive = driveArg;
        targetIntAngle = toDegrees( targetAngleArg );
    }

    @Override
    public void initialize() {
        atCorrectAngle = false;
        System.out.println("==== START TURN =====");        
    }

    @Override
    public void execute() {

        // Compute a target turn rate
        //
        final int currentIntAngle = toDegrees( drive.getRotation().getRadians() );
        final int angleDiff = subtractDegrees( targetIntAngle, currentIntAngle );
        final double motorPower = Math.max(.5, Math.min(1.0, Math.abs(((double) angleDiff) / 100.0)));

        //System.out.println( "Angle " + currentIntAngle + " " + drive.getAngle() );

        //
        // If we're close, exit the command.  TODO: I'm being pretty liberal about close
        // (45 degrees) because there's a lot of drift.
        //
        if ( Math.abs( angleDiff ) < 45 ) {
            atCorrectAngle = true;
            drive.setMotors( 0, 0 );
            return;
        }

        // Set the motors for a tight turn.  This seems to be the most accurate
        //
        if ( angleDiff < 0 ) {
            drive.setMotors( motorPower, -motorPower );
        }
        else {
            drive.setMotors( -motorPower, motorPower );
        }
    }

    @Override
    public void end(boolean interrupted) {
        drive.setMotors(0.0, 0.0);
    }

    @Override
    public boolean isFinished() {
        return atCorrectAngle;
    }
}