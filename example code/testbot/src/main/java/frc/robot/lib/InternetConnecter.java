package frc.robot.lib;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

//This is a Singleton and is global.
public class InternetConnecter{
    private static InternetConnecter instance;

    private Socket socket;
    private OutputStream output;
    public InputStream input;
    private PrintWriter writer;
    private BufferedReader reader;
  
    private boolean isConnected;

    private String pipe;


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

    public static InternetConnecter getInstance(){
        if (instance == null){
            instance = new InternetConnecter();
        }
        return instance;
    }

    public void addToPipe(String data){
        pipe = pipe + data + "\n";
    }

    public void cleanPipe(){
        pipe = "\n";
    }

    public void periodic(){
        if(isConnected && pipe.length() != 0){
            System.out.print(pipe);
            writer.print(pipe);
            writer.flush();
            pipe = "";
        }
        getData();
    }

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