package frc.robot.lib;

public class OctoEncoder{

    private int count = 0;
    private boolean direction = true;
    private double distance = 0.0;
    private double distancePerPulse = 1.0;
    private double rate;
    private int raw = 0;
    private boolean stopped = true;

    private int newTurns;
    private int newSpeed;

    private String name;
    private String robotInput;

    private InternetConnecter internet = InternetConnecter.getInstance();

    public OctoEncoder(String initName){
        name = initName;
        robotInput = "\n";
        internet.addToOutPipe("datasend 1");
    }

    public void periodic(){
        internet.periodic();
        robotInput = internet.getInPipe();

        parseInput(robotInput);
 
        count += newTurns - raw;
        raw = newTurns;
        distance = count * distancePerPulse;
        if(newTurns - raw != 0){
            direction = newTurns - raw > 0;
        }
        stopped = newTurns - raw == 0;

        rate = (double)newSpeed / 65536.0;
    }

    private void parseInput(String input){
        String[] commands = input.split("\n");
        String[] data;
        //System.out.println(input);
        if(commands.length != 0){
            for(int i = commands.length - 1; i >= 0; i--){
                //System.out.println(commands[i]);
                data = commands[i].split(" ");
                if(data.length >= 3 && data[0].equals(name)){
                    //System.out.println("---------------------");
                    try{
                        newTurns = Integer.parseInt(data[1]);
                        newSpeed = Integer.parseInt(data[2]);
                    }
                    catch(NumberFormatException e){
                    }
                    break;
                }
            }
        }
    }

    public int get(){
        return(count);
    }

    public boolean getDirection(){
        return(direction);
    }

    public double getDistance(){
        return(distance);
    }

    public double getDistancePerPulse(){
        return(distancePerPulse);
    }

    public double getRate(){
        return(rate);
    }

    public int getRaw(){
        return(raw);
    }

    public boolean getStopped(){
        return(stopped);
    }

    public void setDistancePerPulse(double newDistancePerPulse){
        distancePerPulse = newDistancePerPulse;
    }

    public void reset(){
        //System.out.print('a');
        count = 0;
    }
}