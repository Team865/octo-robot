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

    public AutonomousCommand(OctoDriveSubsystem drive ) {
        commandList = new SequentialCommandGroup(
            new ForwardCommand( drive, 20.0 ),            
            new TurnCommand   ( drive, Math.PI/2 ),
            new ForwardCommand( drive, 20.0 ),
            new TurnCommand   ( drive, Math.PI ),
            new ForwardCommand( drive, 20.0 ),            
            new TurnCommand   ( drive, 1.5 * Math.PI ),
            new ForwardCommand( drive, 20.0 ),            
            new TurnCommand   ( drive, 0.0 )           
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