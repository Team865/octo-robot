/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.OctoDriveSubsystem;

public class AutonomousCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    private OctoDriveSubsystem octoDrive;

    public AutonomousCommand(OctoDriveSubsystem initSubsystem) {
        octoDrive = initSubsystem;
        addRequirements(initSubsystem);
    }



    @Override
    public void initialize() {
    }



    @Override
    public void execute() {

    }



    @Override
    public void end(boolean interrupted) {
        octoDrive.setMotors(0.0, 0.0);
    }



    @Override
    public boolean isFinished() {
      return false;
    }
}