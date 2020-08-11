import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class RenderPanel extends JPanel implements KeyListener {

    private SwitchController switchController;

    public RenderPanel(){
        this(null);
    }

    public RenderPanel(SwitchController switchController){
        this.switchController = switchController;
        addKeyListener(this);
        setFocusable(true);
        setFocusTraversalKeysEnabled(false);
    }

    public SwitchController getSwitchController() {
        return switchController;
    }

    public void setSwitchController(SwitchController switchController) {
        this.switchController = switchController;
    }

    @Override
    public void keyTyped(KeyEvent e) {

    }

    @Override
    public void keyPressed(KeyEvent e) {
        int[] switchActions = switchController.getSwitchActions();

        switch (e.getKeyCode()){
            case KeyEvent.VK_Z:
                switchActions[SwitchController.SWITCH_A] = 1;
                break;
            case KeyEvent.VK_X:
                switchActions[SwitchController.SWITCH_B] = 1;
                break;
            case KeyEvent.VK_W:
                switchActions[SwitchController.SWITCH_LY] = 0;
                break;
            case KeyEvent.VK_S:
                switchActions[SwitchController.SWITCH_LY] = 255;
                break;
            case KeyEvent.VK_A:
                switchActions[SwitchController.SWITCH_LX] = 0;
                break;
            case KeyEvent.VK_D:
                switchActions[SwitchController.SWITCH_LX] = 255;
                break;
            case KeyEvent.VK_Q:
                switchActions[SwitchController.SWITCH_L] = 1;
                break;
            case KeyEvent.VK_R:
                switchActions[SwitchController.SWITCH_R] = 1;
                break;
            case KeyEvent.VK_E:
                switchActions[SwitchController.SWITCH_X] = 1;
                break;
            case KeyEvent.VK_TAB:
                switchActions[SwitchController.SWITCH_HOME] = 1;
                break;
            case KeyEvent.VK_T:
                switchActions[SwitchController.SWITCH_START] = 1;
                break;

            case KeyEvent.VK_RIGHT:
                switchActions[SwitchController.SWITCH_HAT] = 2;
                break;
            case KeyEvent.VK_UP:
                switchActions[SwitchController.SWITCH_HAT] = 0;
                break;
            case KeyEvent.VK_DOWN:
                switchActions[SwitchController.SWITCH_HAT] = 4;
                break;
            case KeyEvent.VK_LEFT:
                switchActions[SwitchController.SWITCH_HAT] = 6;
                break;




            case KeyEvent.VK_F:
                switchController.setRunMacros(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        SwitchControllerFunctions.walk(switchController);
                    }
                }).start();
                break;
            case KeyEvent.VK_G:
                switchController.setRunMacros(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        SwitchControllerFunctions.recoverSP(switchController);
                    }
                }).start();
                break;
            case KeyEvent.VK_ESCAPE:
                switchController.setRunMacros(false);
                break;
            case KeyEvent.VK_C:
                switchController.setRunMacros(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        SwitchControllerFunctions.walkInCircle(switchController);
                    }
                }).start();
                break;

        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int[] switchActions = switchController.getSwitchActions();

        switch (e.getKeyCode()){
            case KeyEvent.VK_Z:
                switchActions[SwitchController.SWITCH_A] = 0;
                break;
            case KeyEvent.VK_X:
                switchActions[SwitchController.SWITCH_B] = 0;
                break;
            case KeyEvent.VK_W:
            case KeyEvent.VK_S:
                switchActions[SwitchController.SWITCH_LY] = 128;
                break;
            case KeyEvent.VK_A:
            case KeyEvent.VK_D:
                switchActions[SwitchController.SWITCH_LX] = 128;
                break;
            case KeyEvent.VK_Q:
                switchActions[SwitchController.SWITCH_L] = 0;
                break;
            case KeyEvent.VK_R:
                switchActions[SwitchController.SWITCH_R] = 0;
                break;
            case KeyEvent.VK_E:
                switchActions[SwitchController.SWITCH_X] = 0;
                break;
            case KeyEvent.VK_TAB:
                switchActions[SwitchController.SWITCH_HOME] = 0;
                break;
            case KeyEvent.VK_T:
                switchActions[SwitchController.SWITCH_START] = 0;
                break;


            case KeyEvent.VK_RIGHT:
            case KeyEvent.VK_LEFT:
            case KeyEvent.VK_UP:
            case KeyEvent.VK_DOWN:
                switchActions[SwitchController.SWITCH_HAT] = 8;
                break;
        }
    }


}
