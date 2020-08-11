import java.util.Arrays;

public class SwitchController {
    public static final int
            SWITCH_Y       = 0,
            SWITCH_B       = 1,
            SWITCH_A       = 2,
            SWITCH_X       = 3,
            SWITCH_L       = 4,
            SWITCH_R       = 5,
            SWITCH_ZL      = 6,
            SWITCH_ZR      = 7,
            SWITCH_SELECT  = 8,
            SWITCH_START   = 9,
            SWITCH_LCLICK  = 10,
            SWITCH_RCLICK  = 11,
            SWITCH_HOME    = 12,
            SWITCH_CAPTURE = 13,
            SWITCH_LX      = 14,
            SWITCH_LY      = 15,
            SWITCH_RX      = 16,
            SWITCH_RY      = 17,
            SWITCH_HAT     = 18;
    private int[] switchActions = new int[19];
    private boolean runMacros;

    public SwitchController(){
        for (int i = 0; i < 14; i++){
            switchActions[i] = 0;
        }
        for (int i = 14; i < 18; i++){
            switchActions[i] = 128;
        }
        switchActions[SWITCH_HAT] = 8;
    }

    public byte[] getArrBytes(){
        byte[] bytes = new byte[8];
        bytes[0] = 85;
        bytes[1] = 0;
        bytes[2] = 0;

        for (int i = 0; i < 8; i++){
            bytes[1] += (byte)(switchActions[i] * Math.pow(2, i));
        }

        for (int i = 8; i < 14; i++){
            bytes[2] += (byte)(switchActions[i] * Math.pow(2, i - 8));
        }

        bytes[3] = (byte) (switchActions[SWITCH_LX]);
        bytes[4] = (byte) (switchActions[SWITCH_LY]);
        bytes[5] = (byte) (switchActions[SWITCH_RX]);
        bytes[6] = (byte) (switchActions[SWITCH_RY]);
        bytes[7] = (byte) (switchActions[SWITCH_HAT]);

        return bytes;
    }

    public byte[] getArrBytes(byte[] mergeController){
        byte[] bytes = getArrBytes();

        bytes[1] |= mergeController[1];
        bytes[2] |= mergeController[2];

        for (int i = 3; i < 7; i ++){
            if (bytes[i] == (byte) 128)
                bytes[i] = mergeController[i];
        }

        return bytes;
    }


    public int[] getSwitchActions() {
        return switchActions;
    }

    public void setSwitchActions(int[] switchActions) {
        this.switchActions = switchActions;
    }

    public boolean isRunMacros() {
        return runMacros;
    }

    public void setRunMacros(boolean runMacros) {
        this.runMacros = runMacros;
    }

    @Override
    public String toString() {
        return "SwitchController:" +
                "switchActions: " + Arrays.toString(switchActions) +
                "\nButton Values: " + byteToString(getArrBytes()[1]) +
                " " + byteToString(getArrBytes()[2]) +
                "\nLeft Stick: " + switchActions[SWITCH_LX] + " " + switchActions[SWITCH_LY] +
                "\nRight Stick: " + switchActions[SWITCH_RX] + " " + switchActions[SWITCH_RY] +
                "\nByte Array: " + Arrays.toString(getArrBytes());
    }

    public static String byteToString(byte b){
        String output = "";
        int intByte;
        if (b >= 0) {
            intByte = b;
        }
        else {
            intByte = 256 + b;
        }

        for (int i = 7; i >= 0; i--){
            if (intByte >= Math.pow(2, i)){
                output += "1";
                intByte -= Math.pow(2, i);
            }
            else
                output += "0";
        }
        return output;
    }

    public void pressButton(int name, long milliseconds){
        if (name >= SWITCH_CAPTURE || !runMacros)
            return;

        new Thread(new Runnable() {
            @Override
            public void run() {
                switchActions[name] = 1;
                try {
                    Thread.sleep(milliseconds);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                switchActions[name] = 0;
            }
        }).start();
    }

    public void moveStick(int name, long milliseconds, int value){
        if (name < SWITCH_CAPTURE || !runMacros)
            return;

        new Thread(new Runnable() {
            @Override
            public void run() {
                switchActions[name] = value;
                try {
                    Thread.sleep(milliseconds);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                switchActions[name] = 128;
            }
        }).start();
    }
}
