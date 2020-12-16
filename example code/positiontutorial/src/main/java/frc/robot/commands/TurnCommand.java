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
    }

    @Override
    public void execute() {

        // Compute a target turn rate
        //
        final int currentIntAngle = toDegrees( drive.getAngle() );
        final int diff = subtractDegrees( targetIntAngle, currentIntAngle );
        final double diffMag = Math.max(.7, Math.min(1.0, Math.abs(((double) diff) / 50.0)));

        //System.out.println( "Angle " + currentIntAngle + " " + drive.getAngle() );

        // If we're close, exit the command
        //
        if ( Math.abs( diff ) < 10 ) {
            atCorrectAngle = true;
            drive.setMotors( 0, 0 );
            return;
        }

        // Set the motors
        //
        if ( diff < 0 ) {
            drive.setMotors( diffMag, diffMag/2 );
        }
        else {
            drive.setMotors( diffMag/2, diffMag );
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