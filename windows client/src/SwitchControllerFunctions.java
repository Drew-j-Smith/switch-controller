import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Rect;


import java.awt.image.BufferedImage;

public class SwitchControllerFunctions {

    public SwitchControllerFunctions(){}

    private static Rect playerIcon = new Rect(85,45,55,105);
    private static Rect menu = new Rect(1155,645,35,28);
    private static Rect victory = new Rect(102,168,309,97);

    private static Mat fightMat = OpenCVutil.retrieveImage("fight.bmp");
    private static Mat playerIconMat = fightMat.submat(playerIcon);
    private static Mat menuMat = fightMat.submat(menu);

    private static Mat winMat = OpenCVutil.retrieveImage("win.bmp");
    private static Mat victoryMat = winMat.submat(victory);

    private static volatile boolean menuIsShowing;
    private static volatile boolean victoryIsShowing;
    private static volatile boolean playerIconIsShowing;
    private static volatile double cursorY;
    private static int actionCounter = 0;

    public static void setup() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (true){
                    SwitchControllerFunctions.testForImages();
                }
            }
        }).start();
    }


    public static void action(SwitchController switchController){
        if (playerIconIsShowing)
            attack(switchController);
        else
            block(switchController);
    }

    public static void attack(SwitchController switchController){
        actionCounter++;
        try {
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 0);
            Thread.sleep(40);
            switchController.pressButton(SwitchController.SWITCH_R, 40);
            Thread.sleep(40);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(40);
            switchController.pressButton(SwitchController.SWITCH_R, 40);
            Thread.sleep(80);
            switchController.moveStick(SwitchController.SWITCH_LY, 50, 255);
            Thread.sleep(120);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(120);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(12000);
            if (victoryIsShowing)
                victoryAction(switchController);
            else
                flee(switchController);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void walk(SwitchController switchController){
        int[] switchActions = switchController.getSwitchActions();
        switchActions[SwitchController.SWITCH_B] = 1;
        while (switchController.isRunMacros() && !menuIsShowing){
            try {
                switchController.moveStick(SwitchController.SWITCH_LY, 450, 0);
                Thread.sleep(500);
                if (!switchController.isRunMacros() && menuIsShowing)
                    break;
                switchController.moveStick(SwitchController.SWITCH_LY, 450, 255);
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        switchActions[SwitchController.SWITCH_LY] = 128;
        switchActions[SwitchController.SWITCH_B] = 0;
        fixMouseCursor(switchController);
        action(switchController);
    }

    public static void block(SwitchController switchController){
        try {
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 0);
            Thread.sleep(80);
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 0);
            Thread.sleep(40);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            while (!menuIsShowing && !victoryIsShowing) {
                Thread.sleep(500);
            }
            if (switchController.isRunMacros()) {
                if (victoryIsShowing)
                    victoryAction(switchController);
                else
                    action(switchController);
            }
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public static void recoverSP(SwitchController switchController){
        try {
            switchController.pressButton(SwitchController.SWITCH_X, 40);
            Thread.sleep(100);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(300);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(160);
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 255);
            Thread.sleep(160);
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 255);
            Thread.sleep(160);
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 255);
            Thread.sleep(160);
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 255);
            Thread.sleep(160);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(160);
            switchController.pressButton(SwitchController.SWITCH_A, 40);
            Thread.sleep(160);
            switchController.pressButton(SwitchController.SWITCH_X, 40);
            Thread.sleep(1000);
            walk(switchController);
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    public static void fixMouseCursor(SwitchController switchController){
        while (menuIsShowing && cursorY > 20){
            switchController.moveStick(SwitchController.SWITCH_LY, 40, 0);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public static void victoryAction(SwitchController switchController){
            try {
                        switchController.pressButton(SwitchController.SWITCH_A, 40);
                        Thread.sleep(2000);
                        switchController.pressButton(SwitchController.SWITCH_A, 40);
                        Thread.sleep(3000);
                        if (actionCounter >= 2) {
                            actionCounter = 0;
                            recoverSP(switchController);
                        }
                        else
                            walk(switchController);

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
    }

    public static void flee(SwitchController switchController){

            try {
                while (!menuIsShowing)
                    Thread.sleep(500);
                switchController.moveStick(SwitchController.SWITCH_LY, 40, 0);
                Thread.sleep(40);
                switchController.pressButton(SwitchController.SWITCH_A, 40);
                Thread.sleep(6000);
                if (actionCounter >= 2) {
                    actionCounter = 0;
                    recoverSP(switchController);
                }
                else
                    walk(switchController);


            } catch (InterruptedException e) {
                e.printStackTrace();
            }

    }

    public static void walkInCircle(SwitchController switchController){
        int[] switchActions = switchController.getSwitchActions();
        //switchActions[SwitchController.SWITCH_LX] = 255;
        //switchActions[SwitchController.SWITCH_RX] = 0;
        switchActions[SwitchController.SWITCH_B] = 1;
        while (switchController.isRunMacros()){
            try {
                switchController.pressButton(SwitchController.SWITCH_A,50);
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        switchActions[SwitchController.SWITCH_LX] = 128;
        switchActions[SwitchController.SWITCH_RX] = 128;
        switchActions[SwitchController.SWITCH_B] = 0;
    }


    private static boolean[] testForImages(){
        boolean[] booleans = new boolean[3];

        BufferedImage image = OpenCVutil.screenshot();
        Mat screen = OpenCVutil.bufferedImgToMat(image);

//        System.out.println(menu.x + " " + menu.y + " " + menu.width + " " + menu.height);
////        System.out.println(screen.size());
        Mat menuResult = OpenCVutil.matchTemplate(screen.submat(new Rect(menu.x - 20, menu.y - 10, menu.width + 50, menu.height + 280)), menuMat);
        Core.MinMaxLocResult menuMatch = OpenCVutil.findMatchLocation(menuResult);

        if (menuMatch.maxVal > .9) {
            menuIsShowing = true;
            cursorY = menuMatch.maxLoc.y;
        }
        else
            menuIsShowing = false;


        Mat victoryResult = OpenCVutil.matchTemplate(screen.submat(victory), victoryMat);
        Core.MinMaxLocResult victoryMatch = OpenCVutil.findMatchLocation(victoryResult);

        if (victoryMatch.maxVal > .9)
            victoryIsShowing = true;
        else
            victoryIsShowing = false;

        Mat iconResult = OpenCVutil.matchTemplate(screen.submat(playerIcon), playerIconMat);
        Core.MinMaxLocResult iconMatch = OpenCVutil.findMatchLocation(iconResult);

        if (iconMatch.maxVal > .9)
            playerIconIsShowing = true;
        else
            playerIconIsShowing = false;


        return booleans;
    }

}
