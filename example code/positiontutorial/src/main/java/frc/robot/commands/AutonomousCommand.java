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
import frc.robot.commands.ForwardCommand;
import frc.robot.commands.TurnCommand;

public class AutonomousCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    private SequentialCommandGroup commandList;

    static double toRadians( double degrees ) {
        return degrees / 180 * Math.PI;
    }

    public AutonomousCommand(OctoDriveSubsystem drive ) {
        commandList = new SequentialCommandGroup(
            // Forward 1 meter
            new ForwardCommand( drive, 100.0, 0.0 ),
            // Left turn, stop short of 90 degrees because we drift a bit
            new TurnCommand   ( drive, toRadians(75.0) ), 
            // Forward about 50cm
            new ForwardCommand( drive, 100.0, 50.0 ),  
            // Turn around.  Again, stop a bit short of the target angle          
            new TurnCommand   ( drive, toRadians( 190 ) ),      
            // Back to the start.         
            new ForwardCommand( drive, 0.0, 0.0 )  
        );
    }


    @Override
    public void initialize() {
        commandList.initialize();
    }

    @Override
    public void execute() {
        commandList.execute();
    }



    @Override
    public void end(boolean interrupted) {
        commandList.end( interrupted );
    }



    @Override
    public boolean isFinished() {
        return commandList.isFinished();
    }
}