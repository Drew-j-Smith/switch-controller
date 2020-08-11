import com.fazecast.jSerialComm.SerialPort;

import javax.swing.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class SerialInterface {
    private SerialPort serialPort;
    private byte[] bytes;

    public SerialInterface(){
        serialPort = null;
    }

    public SerialInterface(SerialPort serialPort){
        this.serialPort = serialPort;
    }

    public void promptUserForPort(){
        SerialPort[] serialPorts = SerialPort.getCommPorts();
        List<String> serialPortStrings = new ArrayList<>();
        for (SerialPort s : serialPorts){
            serialPortStrings.add(s.getDescriptivePortName());
        }
        String option = (String) JOptionPane.showInputDialog(null, "Choose the port to use:", "", JOptionPane.QUESTION_MESSAGE, null, serialPortStrings.toArray(), 0);

        for (SerialPort s : serialPorts){
            if (s.getDescriptivePortName().equals(option))
                serialPort = s;
        }
    }

    public void init() throws Exception{
        if (serialPort == null)
            throw new Exception("The serial port was not set!");

        serialPort.setComPortParameters(9600,8,1,0);
        serialPort.openPort();
    }

    /*public byte[] writeBytes(byte[] bytes) throws Exception{
        if (!serialPort.isOpen())
            throw new Exception("The serial port was not initialized!");
        byte[] readBytes = new byte[bytes.length];

        serialPort.writeBytes(bytes, bytes.length);
        while (serialPort.bytesAvailable() <= bytes.length-1){}
        serialPort.readBytes(readBytes, bytes.length);

        return readBytes;
    }*/

    public SerialPort getSerialPort() {
        return serialPort;
    }

    public void setSerialPort(SerialPort serialPort) {
        this.serialPort = serialPort;
    }

    public byte[] getBytes() {
        return bytes;
    }

    public void setBytes(byte[] bytes) {
        this.bytes = bytes;
    }

    public void startCommunication() throws Exception{
        if (!serialPort.isOpen())
            throw new Exception("The serial port was not initialized!");

        new Thread(new Runnable() {
            @Override
            public void run() {
                byte[] testByte = new byte[1];

                while (true) {
                    serialPort.writeBytes(bytes, 8);
                    while (serialPort.bytesAvailable() <= 7);

                    while (serialPort.bytesAvailable() > 0){
                        serialPort.readBytes(testByte, 1);
                        System.out.print(Arrays.toString(testByte));
                    }

                    System.out.println(Arrays.toString(bytes));
                }
            }
        }).start();
    }
}