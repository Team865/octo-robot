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

    private String pipe;

    /*
    The init function. It connects to the robot here.
    If the robot is unable to connect for whatever reason it will
    mark isConnected as false, effectivly disabling the class as most
    functions will not run unless is connected is true.
    */
    private InternetConnecter(){
        int trycount = 0;
        isConnected = false;
        pipe = "\n";
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
    public void addToPipe(String data){
        pipe = pipe + data + "\n";
    }

    /*
    Cleans out the pipe, canceling any stored commands.
    */
    public void cleanPipe(){
        pipe = "\n";
    }

    /*
    This is not automaticly called and must be called by
    another periodic function. If there is text stored in
    the pipe, send it to the robot. Then get any data that
    the robot is trying to send back.
    */
    public void periodic(){
        if(isConnected && pipe.length() != 0){
            System.out.print(pipe);
            writer.print(pipe);
            writer.flush();
            pipe = "";
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
                    System.out.print((char)reader.read());
                }
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}