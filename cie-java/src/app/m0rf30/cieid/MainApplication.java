package app.m0rf30.cieid;

import app.m0rf30.cieid.util.Utils;
import java.awt.AWTEvent;
import java.awt.EventQueue;
import java.awt.Toolkit;
import java.awt.event.AWTEventListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import javax.swing.JFrame;

public class MainApplication {

    private JFrame frame;

    /** Launch the application. */
    public static void main(final String[] args) {
        EventQueue.invokeLater(
                new Runnable() {
                    public void run() {
                        installKeyboardMonitor();
                        if (args.length > 0 && args[0].equals("pinwrong")) {
                            sendNotification("PIN Errato");
                        } else if (args.length > 0 && args[0].equals("cardnotregistered")) {
                            sendNotification("Carta non abbinata, premere qui per abbinare la CIE");
                        } else if (args.length > 0 && args[0].equals("pinlocked")) {
                            sendNotification(
                                    "Carta bloccata, premere qui per sbloccarla con il PUK");
                        } else {
                            try {
                                showUI(args);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
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

    private static void sendNotification(String message) {
        // Build the command arguments.
        // You may add extra flags as required (e.g., -u for urgency, -t for timeout, etc.)
        String[] command = {
            "notify-send", "-a", "Cie ID", "-i", "app.m0rf30.cieid", message,
        };

        ProcessBuilder processBuilder = new ProcessBuilder(command);
        try {
            // Start the process
            Process process = processBuilder.start();
            // Optionally wait for the process to complete
            int exitCode = process.waitFor();
            if (exitCode != 0) {
                System.err.println("Notification command failed with exit code: " + exitCode);
            }
        } catch (IOException e) {
            System.err.println("IOException while trying to send notification:");
            e.printStackTrace();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.err.println("Notification process was interrupted:");
            e.printStackTrace();
        }
    }
}
