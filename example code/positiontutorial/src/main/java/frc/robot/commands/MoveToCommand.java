/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import edu.wpi.first.wpilibj.geometry.*;
import frc.robot.subsystems.OctoDriveSubsystem;
import frc.robot.subsystems.OdometrySubsystem;

/**
 * Move to a desired location in 2D space
 */
public class MoveToCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    // The drive we use to set motor speeds
    private OctoDriveSubsystem drive;

    // The source of our odometry (robot position & orientation)
    private OdometrySubsystem odometry;

    // The location in 2D space we're trying to get to.  Units are meters
    private Translation2d desiredTranslation;

    // Set to true if we've reached our target
    private boolean isFinishedFlag = false;

    /**
     * Move to a desired location in 2D space (targetArg).
     * 
     * @param driveArg     The drive system.  Used to get odometry and to set motor speeds
     * @param odometryArg  Used to get the current locaton of the Robot.
     * @param targetArg    The target location we're trying to get to.  Units are Meters.
     * 
     * Limitations:  The expection is that we start pointing toward the desired location.  If 
     * we don't the command should still take the robot to the desired location, but it may
     * follow a large, sweeping arch
     * 
     */
    public MoveToCommand(OctoDriveSubsystem driveArg, OdometrySubsystem odometryArg, Translation2d targetArg ) {
        drive = driveArg;
        odometry = odometryArg;
        desiredTranslation = targetArg;
        addRequirements( drive, odometry );
    }

    /**
     * Standard command initialize.  We should just need to reset the isFinishedFlag;
     * the Move To command is mostly stateless.
     */
    @Override
    public void initialize() {
        isFinishedFlag = false;
    }

    /*
     * Implementation of the command excute.
     * 
     * 1. Compute the translation to the target location & distance left to travel
     * 2. Halt & declare victory if we're close to the targe
     * 3. Compute the desired heading (desiredRotation)
     * 4. Compute the rotation change needed to get to that heading (correctionRotation)
     * 5. Figure out the degree change needed to get back on course (correctionDegrees)
     * 6. Compute motor power change needed to make the course adjustment
     * 7. Set the motor power
     */
    @Override
    public void execute() {

        //
        // 1. Compute the translation to the target location & distance left to travel
        //
        // dTarget == how do we change our current position to get to the target
        //
        final Pose2d position = odometry.getPoseMeters();
        final Translation2d currentTranslation = position.getTranslation();
        final Translation2d translationToTarget = desiredTranslation.minus( currentTranslation );
        final double distanceLeft = translationToTarget.getNorm();       

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
        final Rotation2d desiredRotation = new Rotation2d( translationToTarget.getX(), translationToTarget.getY() );
        
        //
        // 4. Compute the rotation change needed to get to that heading (correctionRotation)
        //
        final Rotation2d currentRotation = position.getRotation();
        final Rotation2d correctionRotation = desiredRotation.minus( currentRotation );

        //
        // 5. Figure out the degree change needed to get back on course (correctionDegrees)
        //
        // - a value of 15 means we need to change heading 15 degrees to the left
        // - a value of -15 means we need to change heading 15 degrees to the right
        // - All angles follow right hand rule.  https://en.wikipedia.org/wiki/Right-hand_rule
        //
        final double rawCorrectionDegrees = correctionRotation.getDegrees();
        // Map angle from 0-360 degrees to -180 to 180 degrees.
        final double correctionDegrees = rawCorrectionDegrees > 180 ? rawCorrectionDegrees - 360 : rawCorrectionDegrees;

        // 
        // 6. Compute the motor power change needed to make the course adjustment
        // 
        // We'll gradulally turn toward the target by under-powering one of our motors.  The computations
        // below are "hueristics".  The math was fiddled with until the resulting robot path
        // looked reasonable.  TODO - this probably translates into a PID of some kind, and a PID
        // would is a clearer way to communicate the intent of the code.
        // 
        // Increase the amount we're willing to adjust the angle when we get closer to target
        final double closeToTargetMul =Math.max( 1.0, 3.0 - distanceLeft * 10 );
        // Compute the magnitude of the current error.  Adjust more when error is larger
        final double currentErrorMul = Math.abs( correctionDegrees );
        // Declare a constant multiplier.  Lowering this gives smoother corrections (but you may miss the target)
        final double constantMul = 0.05f;
        // Compute a raw weak motor power by subtracting 1 (full power) from the product of our multipliers
        final double rawWeakerMotorPower = 1.0 - constantMul * currentErrorMul * closeToTargetMul;
        // Limit the weaker motor power to .7 so we don't get crazy turns
        final double weakerMotorPower = Math.max( rawWeakerMotorPower, 0.7 );

        // 
        // 7. Set the motor power
        //
        if ( correctionDegrees < 0 ) {
            // A <0 correction is a right turn. Full power left motor & under power right
            drive.setMotors( 1.0, weakerMotorPower );
        }
        else {    
            // A >0 correction is a left turn.  Full power right motor & under power left
            drive.setMotors( weakerMotorPower, 1.0 );          
        }
    }

    /**
     * "end" implementation.  Just come to a stop
     */
    @Override
    public void end(boolean interrupted) {
        drive.setMotors(0.0, 0.0);
    }

    /**
     * isFinished implementation.  Return the isFinishedFlag that execute set.
     */
    @Override
    public boolean isFinished() {
        return isFinishedFlag;
    }
}