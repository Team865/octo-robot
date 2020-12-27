/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package frc.robot.lib;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

/*
InternetConnecter's job is to dirrectly communicate with the robot.
This class exists due to the fact that the octorobot does not have
a roboRIO and must be talked to using wifi. This is a singleton class.
*/
public class InternetConnecter{
    private static InternetConnecter instance;

    private Socket socket;
    private OutputStream output;
    public InputStream input;
    private PrintWriter writer;
    private BufferedReader reader;
  
    private boolean isConnected;

    private String outpipe;
    private String inpipe;

    /*
    The init function. It connects to the robot here.
    If the robot is unable to connect for whatever reason it will
    mark isConnected as false, effectivly disabling the class as most
    functions will not run unless is connected is true.
    */
    private InternetConnecter(){
        int trycount = 0;
        isConnected = false;
        outpipe = "\n";
        inpipe = "\n";
        while (trycount <= 3 && !isConnected){
            try {
                socket = new Socket("192.168.4.1", 4999);
                output = socket.getOutputStream();
                input = socket.getInputStream();
                writer = new PrintWriter(output, true);
                reader = new BufferedReader(new InputStreamReader(input));
                isConnected = true;
            }     
            catch (IOException e) {
                e.printStackTrace();
                trycount++;
            }
        }
    }

    /*
    Gets the instance of the internet connector. Due to
    this class being a singleton, you must call this function
    to get an InternetConnector. There will only ever be
    one InternetConnector.
    */
    public static InternetConnecter getInstance(){
        if (instance == null){
            instance = new InternetConnecter();
        }
        return instance;
    }

    /*
    Adds commands to be sent to the robot. Does not send them.
    */
    public void addToOutPipe(String data){
        outpipe = outpipe + data + "\n";
    }

    public String getInPipe(){
        if(inpipe.length() > 100){
            inpipe.substring(50);
        }
        return(inpipe);
    }

    /*
    Cleans out the pipe, canceling any stored commands.
    */
    public void cleanInPipe(){
        inpipe = "\n";
    }

    public void cleanOutPipe(){
        outpipe = "\n";
    }

    /*
    This is not automaticly called and must be called by
    another periodic function. If there is text stored in
    the pipe, send it to the robot. Then get any data that
    the robot is trying to send back.
    */
    public void periodic(){
        if(isConnected && outpipe.length() != 0){
            //System.out.print(outpipe);
            writer.print(outpipe);
            writer.flush();
            outpipe = "";
        }
        getData();
    }


    /*
    Gets the data that the robot is trying to send back.
    It will get and print all the data that the robot has.
    */
    public void getData(){
        if(isConnected){
            try {
                while(input.available() > 0){
                    char readChar = (char)reader.read();
                    inpipe = inpipe + readChar;
                    //System.out.print(readChar);
                }
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}