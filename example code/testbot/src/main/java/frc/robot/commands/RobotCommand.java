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

import edu.wpi.first.wpilibj2.command.CommandBase;

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
    //System.out.println("It's time to get funky...");
    motorASpeed = 0;
    motorBSpeed = 0;
    nextASpeed = 0;
    nextBSpeed = 0;
    newSpeeds = true;

    int trycount = 0;
    isConnected = false;
    while (trycount < 10 && !isConnected){
      try {
        socket = new Socket("192.168.4.1", 4999);
        output = socket.getOutputStream();
        input = socket.getInputStream();
        writer = new PrintWriter(output, true);
        reader = new BufferedReader(new InputStreamReader(input));
        isConnected = true;
      } catch (IOException e) {
        //System.out.println("helpmeoutican'tseemtogetthiswindowopen");
        e.printStackTrace();
        trycount++;
      }
      //System.out.println("Never mind now it's open");
    }
  }

  // Called every time the scheduler runs while the command is scheduled.
  @Override
  public void execute() {
    setSpeeds(nextASpeed, nextBSpeed);

    if (isConnected){
      //System.out.println("BREAKDOWN BREAKDOWN");
      if (newSpeeds){
        writer.println("motora=" + motorASpeed);
        writer.println("motorb=" + motorBSpeed);
        newSpeeds = false;
        //System.out.println("I think my hand is broken");
      }
      try {
        //System.out.println("no crash pls :3");
        if(input.available() > 0){
          reader.read();
        }
        //System.out.println("out");       
      } catch (IOException e) {
        //System.out.println("the odds of me seeing this message are high");
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
