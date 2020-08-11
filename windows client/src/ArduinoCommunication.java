import org.opencv.core.Core;

import javax.swing.*;
import java.io.File;



public class ArduinoCommunication {

    public static void main(String[] args) throws Exception {
        //System.loadLibrary( Core.NATIVE_LIBRARY_NAME );


        try {
            String jarLocation = new File(ArduinoCommunication.class.getProtectionDomain()
                    .getCodeSource().getLocation().toURI()).getPath();

            String[] folders = jarLocation.split("\\\\");
            jarLocation = "";

            for (int i = 0; i < folders.length-1; i++){
                jarLocation += folders[i] + "\\";
            }



            System.load(jarLocation + Core.NATIVE_LIBRARY_NAME + ".dll");


        } catch (java.lang.UnsatisfiedLinkError e){
            e.printStackTrace();

            try {
                System.loadLibrary( Core.NATIVE_LIBRARY_NAME );
            }
            catch (java.lang.UnsatisfiedLinkError ex){
                JOptionPane.showMessageDialog(null, e.getMessage());
                e.printStackTrace();
            }

        }


        SerialInterface serialInterface = new SerialInterface();
        serialInterface.promptUserForPort();
        serialInterface.init();


        SwitchControllerFunctions.setup();

        SwitchController switchController = new SwitchController();


        serialInterface.setBytes(switchController.getArrBytes());
        serialInterface.startCommunication();


        JFrame theGui = new JFrame();
        theGui.setSize(80,60);
        theGui.setLocation(500,0);
        theGui.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        RenderPanel renderPanel = new RenderPanel(switchController);
        theGui.add(renderPanel);

        new Thread(new Runnable() {
            @Override
            public void run() {
                byte[] bytes = switchController.getArrBytes();
                byte[] testBytes;
                while (true){
                    testBytes = switchController.getArrBytes();
                    if (testBytes != bytes) {
                        serialInterface.setBytes(switchController.getArrBytes());
                        bytes = testBytes;
                    }
                }
            }
        }).start();

        theGui.setVisible(true);
    }
}
