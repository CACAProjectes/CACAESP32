package es.xuan.com;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * This class implements java socket client
 * @author Juanje
 *
 */
public class Cliente {
	static String strHostname = "192.168.4.1";
	static int iPort = 80;

    public static void main(String[] args) throws UnknownHostException, IOException, ClassNotFoundException, InterruptedException{
        //get the localhost IP address, if server is running on some other IP, you need to use that
        //InetAddress host = InetAddress.getLocalHost();
        Socket socket = null;
        ObjectOutputStream oos = null;
        ObjectInputStream ois = null;
        //for(int i=0; i<5;i++){
            //establish socket connection to server
            //socket = new Socket(host.getHostName(), iPort);
            socket = new Socket(strHostname, iPort);
            //write to socket using ObjectOutputStream
            oos = new ObjectOutputStream(socket.getOutputStream());
            System.out.println("Sending request to Socket Server");
            //if(i==4)oos.writeObject("exit");
            //else oos.writeObject(""+i);
            oos.writeObject("/H\n");
            //read the server response message
            ois = new ObjectInputStream(socket.getInputStream());
            String message = (String) ois.readObject();
            System.out.println("Message: " + message);
            //close resources
            ois.close();
            oos.close();
            //Thread.sleep(100);
        //}
    }
}
