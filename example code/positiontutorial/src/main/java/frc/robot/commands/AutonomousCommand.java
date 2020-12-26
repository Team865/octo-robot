/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;
import frc.robot.subsystems.OctoDriveSubsystem;
import frc.robot.commands.MoveToCommand;
import frc.robot.commands.TurnCommand;
import edu.wpi.first.wpilibj.geometry.*;

/**
 * Demo autonomous command
 * 
 * Uses the WPILib SequentialCommandGroup to execute a sequence of commands that move the robot in a triangle
 */
public class AutonomousCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    private SequentialCommandGroup commandList;

    /**
     * Autonomous mode constructor.  Sets the sequence of commands we want the robot to follow.
     * @param drive This is the Robot's drive.  Used to get odometry and set motor speeds.
     */
    public AutonomousCommand(OctoDriveSubsystem drive ) {
        // A sequence of commands to move the robot autonomously in a triangle.
        // The robot returns to its start position
        commandList = new SequentialCommandGroup(
            // Move to coordinate <100,0>.  The robot starts at <0,0>, facing
            // the X axis, so this is essentially a move forward
            new MoveToCommand( drive, new Translation2d( 100.0, 0.0 ) ),
            // Sharp Left turn to get us close to 90 degrees
            new TurnCommand   ( drive, new Rotation2d(Math.toRadians(75.0)) ),
            // The robot should now be at <100,0>, facing close to 90 degrees.  Going
            // forward now will move the robot along the Y axis.   
            // A "Move To" <100,50> command should move the robot forward about 50cm 
            new MoveToCommand( drive, new Translation2d(100.0, 50.0 )),  
            // Another sharp turn.  This will turn the robot back toward where it started
            // (just over 180 degrees)          
            new TurnCommand   ( drive, new Rotation2d(Math.toRadians( 190 )) ),      
            // Move back to the start.    
            new MoveToCommand( drive, new Translation2d(0.0, 0.0 ))  
        );
    }

    /**
     * Standard command initialize.  Call through to commandList.initialize()
     */
    @Override
    public void initialize() {
        commandList.initialize();
    }

    /**
     * Standard command execute.  Call through to commandList.execute()
     */
    @Override
    public void execute() {
        commandList.execute();
    }

    /**
     * Standard command end.  Call through to commandList.end()
     */
    @Override
    public void end(boolean interrupted) {
        commandList.end( interrupted );
    }

    /**
     * Standard command isFinished.  Call through to commandList.isFinished()
     */
    @Override
    public boolean isFinished() {
        return commandList.isFinished();
    }
}