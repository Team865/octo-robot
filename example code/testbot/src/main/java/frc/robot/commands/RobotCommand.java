/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.commands;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj2.command.CommandBase;
import frc.robot.lib.TestbotSpeedController;

/**
 * An example command that uses an example subsystem.
 */
public class RobotCommand extends CommandBase {
  @SuppressWarnings({ "PMD.UnusedPrivateField", "PMD.SingularField" })
  private Socket socket;
  private OutputStream output;
  public InputStream input;
  private PrintWriter writer;
  private BufferedReader reader;
  
  private boolean isConnected;

  private boolean newSpeeds;
  public static int nextASpeed;
  public static int nextBSpeed;
  private int motorASpeed;
  private int motorBSpeed;
  private TestbotSpeedController motorA;
  private TestbotSpeedController motorB;
  private DifferentialDrive drive;

  /**
   * Creates a new ExampleCommand.
   *
   * @param subsystem The subsystem used by this command.
   */
  public RobotCommand() {
    // Use addRequirements() here to declare subsystem dependencies.
  }

  // Called when the command is initially scheduled.
  @Override
  public void initialize() {
    motorASpeed = 0;
    motorBSpeed = 0;
    nextASpeed = 0;
    nextBSpeed = 0;
    newSpeeds = true;

    int trycount = 0;
    isConnected = false;
    while (trycount < 5 && !isConnected){
      try {
        socket = new Socket("192.168.4.1", 4999);
        output = socket.getOutputStream();
        input = socket.getInputStream();
        writer = new PrintWriter(output, true);
        reader = new BufferedReader(new InputStreamReader(input));
        isConnected = true;

        motorA = new TestbotSpeedController(writer, "motora");
        motorB = new TestbotSpeedController(writer, "motorb");
        drive = new DifferentialDrive(motorA, motorB);
      } catch (IOException e) {
        e.printStackTrace();
        trycount++;
      }
    }
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    setSpeeds(nextASpeed, nextBSpeed);

    if (isConnected){
      drive.tankDrive(motorASpeed, motorBSpeed);
      try {
        if(input.available() > 0){
          reader.read();
        } 
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }

  // Called once the command ends or is interrupted.
  @Override
  public void end(boolean interrupted) {
  }

  // Returns true when the command should end.
  @Override
  public boolean isFinished() {
    return false;
  }

  private void setSpeeds(int aSpeed, int bSpeed){
    if (aSpeed != motorASpeed || bSpeed != motorBSpeed){
      motorASpeed = aSpeed;
      motorBSpeed = bSpeed;
      newSpeeds = true;
    }
  }
}
