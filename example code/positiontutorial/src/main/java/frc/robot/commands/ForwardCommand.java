package frc.robot.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.subsystems.OctoDriveSubsystem;

public class ForwardCommand extends CommandBase {
    @SuppressWarnings({"PMD.UnusedPrivateField", "PMD.SingularField"})
    
    private OctoDriveSubsystem drive;
    private double targetDistance;
    //private double startX, startY, startAngle;
    //private double targetX, targetY;
    private int count;

    public ForwardCommand(OctoDriveSubsystem driveArg, double targetDistanceArg) {
        drive = driveArg;
        targetDistance = targetDistanceArg;
    }

    @Override
    public void initialize() {
        //startX = drive.getX();
        //startY = drive.getY();
        //startAngle = drive.getAngle();
        //targetX = startX + targetDistance * Math.cos( startAngle );        
        //targetY = startY + targetDistance * Math.sin( startAngle );
        count = 0;
    }

    @Override
    public void execute() {
        //final double deltaX = targetX - drive.getX();
        //final double deltaY = targetY - drive.getY();
        //final double distanceLeft = Math.sqrt( deltaX * deltaX + deltaY * deltaY );
        //final double currentAngle = Math.atan2( deltaY, deltaX );       
        //System.out.println( "Desired Angle " + startAngle*180/Math.PI + " Current " + currentAngle*180/Math.PI );
        //System.out.println( "deltaX " + deltaX + " deltay " + deltaY );
        //int difference = ((int) ((currentAngle - startAngle ) * 180 / Math.PI));
        //difference = (1000*360 + difference) % 360;
        //if ( difference > 180 ) {
        //    difference -= 360;
        //}

        if ( count < 60 ) {
            drive.setMotors(1.0, 1.0);
        }
        else {
            drive.setMotors(0.0, 0.0);
        }

        ++count;
    }

    @Override
    public void end(boolean interrupted) {
        drive.setMotors(0.0, 0.0);
    }

    @Override
    public boolean isFinished() {
        if ( count > 90 ) {
            return true;
        }
        return (false);
    }
}