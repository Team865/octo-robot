/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.OctoDriveSubsystem;
import frc.robot.subsystems.Odometry;
import edu.wpi.first.wpilibj.geometry.*;

/**
 * A command that performs a sharp turn
 */
public class TurnCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    // The drive.  Used to set motor power
    private OctoDriveSubsystem drive;
    // The odometry tracker.  Used to get position & orientation
    private Odometry odometry;
    // The desired rotation - what we're trying to achieve
    private Rotation2d desiredRotation;
    // Set to true when we're close to the desired rotation
    private boolean isFinishedFlag = false;

    /**
     * Command to turn toward a desired rotation
     * @param driveArg              The drive.  Used to set motor power
     * @param odometryArg           Odometry tracker.  Used to figure out where we are
     * @param desiredRotationArg    The desired rotation
     */
    public TurnCommand( OctoDriveSubsystem driveArg, Odometry odometryArg, Rotation2d desiredRotationArg) {
        drive = driveArg;
        odometry = odometryArg;
        desiredRotation = desiredRotationArg;
        addRequirements( drive, odometry );
    }

    /**
     * initialize implementation.  The command is mostly stateless so this just resets the finished flag.
     */
    @Override
    public void initialize() {
        isFinishedFlag = false; 
    }

    /**
     * execute implementation
     * 
     * 1. Compute the change we need to make to get to the desired heading (correctionRotation)
     * 2. Figure out the degree change needed to get to our target(correctionDegrees)
     * 3. If we're close (30 degrees), declare victory & exit the command
     * 4. Compute how much power we want to give the motors while turning 
     * 5. Apply power to the motors
     */
    @Override
    public void execute() {

        //
        // 1. Compute the change we need to make to get to the desired heading (correctionRotation)
        //
        final Rotation2d currentRotation = odometry.getPoseMeters().getRotation();
        final Rotation2d correctionRotation = desiredRotation.minus( currentRotation );

        //
        // 2. Figure out the degree change needed to get to our target(correctionDegrees)
        //
        // a value of "15" means we need to change heading 15 degrees to the the left
        // All angles follow right hand rule, so you can also think of this as 15 degrees
        // counter clockwise.  A value of -15 = change heading 15 degrees to the right. 
        //
        final double rawCorrectionDegrees = correctionRotation.getDegrees();
        // getDegrees gets a value from 0 - 360.  The angle needs to be -180 to 180.
        final double correctionDegrees = rawCorrectionDegrees > 180 ? rawCorrectionDegrees - 360 : rawCorrectionDegrees;

        //
        // 3. If we're close (30 degrees), declare victory & exit the command
        //
        if ( Math.abs( correctionDegrees ) < 30 ) {
            isFinishedFlag = true;
            drive.setMotors( 0, 0 );
            return;
        }

        //
        // 4. Compute how much power we want to give the motors while turning 
        //
        // The closer we get to the correct angle, the more we should lower the power.
        // This basically says that a 90 degree difference is full power, then we start scaling down.
        // Limit the power range to a minimmum of 0.4 (40% power) to 1.0 (100% power).
        //
        // These values are basically hueristics & can be adjusted.  They're partly values that seem
        // to give us good odometry.
        //
        final double rawMotorPower = Math.abs(correctionDegrees) / 90;
        final double motorPower = Math.max(0.4, Math.min(1.0, rawMotorPower ));


        //
        // 5. Apply power to the motors
        // 
        // Tight turns seem to get the best result WRT the current odometry's angle accuracy.
        //
        if ( correctionDegrees < 0 ) {
            // Sharp right turn, so full power left motor, reverse right
            drive.setMotors( motorPower, -motorPower );
        }
        else {
            // Sharp left turn, so full power right motor, reverse left
            drive.setMotors( -motorPower, motorPower );
        }
    }

    /**
     * end implementation.  Cut power to the motors.
     */
    @Override
    public void end(boolean interrupted) {
        drive.setMotors(0.0, 0.0);
    }

    /**
     * isFinished implementation.  Return isFinishedFlag set by Execute.
     */
    @Override
    public boolean isFinished() {
        return isFinishedFlag;
    }
}