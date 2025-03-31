package app.m0rf30.cieid;

import app.m0rf30.cieid.util.Utils;
import java.awt.AWTEvent;
import java.awt.EventQueue;
import java.awt.Toolkit;
import java.awt.event.AWTEventListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;

public class MainApplication {

    private JFrame frame;

    /** Launch the application. */
    public static void main(final String[] args) {
        EventQueue.invokeLater(
                new Runnable() {
                    public void run() {
                        installKeyboardMonitor();
                        try {
                            showUI(args);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                });
    }

    /** Close the application on ctrl+q */
    public static void installKeyboardMonitor() {
        Toolkit.getDefaultToolkit()
                .addAWTEventListener(
                        new AWTEventListener() {
                            @Override
                            public void eventDispatched(AWTEvent event) {
                                KeyEvent ke = (KeyEvent) event;
                                if (ke.getID() == KeyEvent.KEY_PRESSED) {
                                    if (ke.getKeyCode() == KeyEvent.VK_Q) {
                                        if (ke.isControlDown()) System.exit(0);
                                    }
                                }
                            }
                        },
                        AWTEvent.KEY_EVENT_MASK);
    }

    public static void showUI(String[] args) {
        MainApplication window = new MainApplication(args);
        window.frame.setVisible(true);
    }

    /** Create the application. */
    public MainApplication(String[] args) {
        initialize(args);
    }

    /** Initialize the contents of the frame. */
    private void initialize(String[] args) {
        if ("false".equals(Utils.getProperty("nomore", "false"))) {
            frame = new IntroFrame();
        } else {
            frame = new MainFrame(args);
        }

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.addWindowListener(
                new WindowAdapter() {
                    @Override
                    public void windowClosed(WindowEvent e) {
                        System.exit(0);
                    }
                });
    }
}
