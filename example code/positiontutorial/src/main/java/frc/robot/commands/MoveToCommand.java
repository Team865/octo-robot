package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import edu.wpi.first.wpilibj.geometry.*;
import frc.robot.subsystems.OctoDriveSubsystem;

/**
 * Command that moves the rebot to a desired location in 2D space
 */
public class MoveToCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    // The drive we use to get odometry from and set motor speeds
    private OctoDriveSubsystem drive;

    // The location in 2D space we're trying to get to.
    private Translation2d desiredTranslation;

    // Set to try if we've reached our target
    private boolean isFinishedFlag = false;

    /**
     * Move to a desired location in 2D space (targetArg).
     * 
     * @param driveArg   The drive system.  Used to get odometry and to set motor speeds
     * @param targetArg  The target location we're trying to get to.  Units are CM.
     * 
     * Limitations:  The expection is that our start facing is relatively close to 
     * the target.  If it isn't, the command should still take the robot to the target,
     * but it may do so in a large, sweeping arch.
     * 
     */
    public MoveToCommand(OctoDriveSubsystem driveArg, Translation2d targetArg ) {
        drive = driveArg;
        desiredTranslation = targetArg;
    }

    /**
     * Standard command initialize.  We should just need to reset the isFinishedFlag;
     * the command is mostly stateless.
     */
    @Override
    public void initialize() {
        isFinishedFlag = false;
    }

    /*
     * Implementation of the command excute.
     * 
     * 1. Compute the delta to the target position & distance left to travel
     * 2. Halt & declare victory if we're close to the targe
     * 3. Compute the desired heading (desiredRotation)
     * 4. Compute the change we need to make to get to that heading (correctionRotation)
     * 5. Figure out the degree change needed to get back on course (correctionDegrees)
     * 6. Compute motor power change needed to make the course adjustment
     * 7. Set the motor power
     */
    @Override
    public void execute() {

        //
        // 1. Compute the delta to the target position & distance left to travel
        //
        // dTarget == how do we change our current position to get to the target
        //
        final Pose2d position = drive.getPoseMeters();
        final Translation2d currentTranslation = position.getTranslation();
        final Translation2d deltaTarget = desiredTranslation.minus( currentTranslation );
        final double distanceLeft = deltaTarget.getNorm();       

        //
        // 2. Halt & declare victory if we're close to the target (10cm)
        //
        if ( distanceLeft < 0.10 ) {
            isFinishedFlag = true;
            drive.setMotors( 0.0, 0.0 );
            return;
        }
       
        //
        // 3. Compute the desired heading (desiredRotation)
        //
        final Rotation2d desiredRotation = new Rotation2d( deltaTarget.getX(), deltaTarget.getY() );
        
        //
        // 4. Compute the change we need to make to get to that heading (correctionRotation)
        //
        final Rotation2d currentRotation = position.getRotation();
        final Rotation2d correctionRotation = desiredRotation.minus( currentRotation );

        //
        // 5. Figure out the degree change needed to get back on course (correctionDegrees)
        //
        // a value of "15" means we need to change heading 15 degrees to the the left
        // All angles follow right hand rule, so you can also think of this as 15 degrees
        // counter clockwise.  A value of -15 = change heading 15 degrees to the right. 
        //
        final double rawCorrectionDegrees = correctionRotation.getDegrees();
        // getDegrees gets a value from 0 - 360.  The angle needs to be -180 to 180.
        final double correctionDegrees = rawCorrectionDegrees > 180 ? rawCorrectionDegrees - 360 : rawCorrectionDegrees;

        // 
        // 6. Compute motor power change needed to make the course adjustment
        // 
        // We'll gradulally turn toward the target by under-powering one of our motors.  The computations
        // below are really hueristics.  The math was fiddled with until the resulting robot path
        // looked reasonable.  TODO - this probably translates into a PID of some kind, and a PID
        // would is a clearer way to communicate the intent of the code.
        // 
        // Increase the amount we're willing to adjust the angle when we get closer to target
        final double distanceAdjustmentMultiplier =Math.max( 1.0, 3.0 - distanceLeft * 10 );
        // Figure out the manginutude of the angular error
        final double correctionMagnitude = Math.abs( correctionDegrees );
        // Compute a raw weak motor power by subtracting 1, full power, from the mangitude of the turn we want to make
        final double rawWeakerMotorPower = 1.0 - 0.05 * correctionMagnitude * distanceAdjustmentMultiplier;
        // Limit the weaker motor power to .7 so we don't get crazy turns
        final double weakerMotorPower = Math.max( rawWeakerMotorPower, 0.7 );

        // 
        // 7. Set the motor power
        //
        if ( correctionDegrees < 0 ) {
            // Less than 0 is a right turn, so full power the left motor & under power the right
            drive.setMotors( 1.0, weakerMotorPower );
        }
        else {    
            // More than 0 is a left turn, so full power the right motor & under power the left
            drive.setMotors( weakerMotorPower, 1.0 );          
        }
    }

    /**
     * End implementation.  Just come to a stop
     */
    @Override
    public void end(boolean interrupted) {
        drive.setMotors(0.0, 0.0);
    }

    /**
     * isFinished implementation.  Return the flag that execute set.
     */
    @Override
    public boolean isFinished() {
        return isFinishedFlag;
    }
}