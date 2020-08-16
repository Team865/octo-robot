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


    private InternetConnecter(){
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

    public void sendData(String data){
        if(isConnected){
            writer.println(data);
        }
    }

    public void getData(){
        if(isConnected){
            try {
                if(input.available() > 0){
                    reader.read();
                } 
            } 
            catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}