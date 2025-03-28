package app.m0rf30.cieid;

import app.m0rf30.cieid.util.Utils;
import ch.swingfx.twinkle.NotificationBuilder;
import ch.swingfx.twinkle.event.NotificationEvent;
import ch.swingfx.twinkle.event.NotificationEventAdapter;
import ch.swingfx.twinkle.style.closebutton.NullCloseButton;
import ch.swingfx.twinkle.style.theme.LightDefaultNotification;
import ch.swingfx.twinkle.window.Positions;
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

                        if (args.length > 0 && args[0].equals("pinwrong")) {
                            notifyPinWrong();
                        } else if (args.length > 0 && args[0].equals("cardnotregistered")) {
                            notifyCardNotRegistered();
                        } else if (args.length > 0 && args[0].equals("pinlocked")) {
                            notifyPinLocked();
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

    private static void notifyPinWrong() {
        NotificationBuilder nb = createNotificationBuilder();
        nb.withTitle("CIE ID");
        nb.withMessage("PIN errato");
        // nb.withIcon(CrmIcons.CALL);
        nb.withDisplayTime(1000 * 5);

        // nb.withListener(new NotificationEventAdapter() {
        // @Override
        // public void clicked(NotificationEvent event)
        // {
        // // do nothing
        // setLookAndFeel();
        // MainApplication.showUI(new String[] {});
        // }
        // });

        nb.showNotification();
    }

    private static void notifyCardNotRegistered() {
        NotificationBuilder nb = createNotificationBuilder();
        nb.withTitle("CIE ID");
        nb.withMessage("Carta non abbinata, premere qui per abbinare la CIE");
        // nb.withIcon(CrmIcons.CALL);
        nb.withDisplayTime(1000 * 10);

        nb.withListener(
                new NotificationEventAdapter() {
                    @Override
                    public void clicked(NotificationEvent event) {
                        // setLookAndFeel();
                        MainApplication.showUI(new String[] {});
                    }
                });

        nb.showNotification();
    }

    private static void notifyPinLocked() {
        NotificationBuilder nb = createNotificationBuilder();
        nb.withTitle("CIE ID");
        nb.withMessage("Carta bloccata, premere qui per sbloccarla con il PUK");
        // nb.withIcon(CrmIcons.CALL);
        nb.withDisplayTime(1000 * 10);

        nb.withListener(
                new NotificationEventAdapter() {
                    @Override
                    public void clicked(NotificationEvent event) {
                        MainApplication.showUI(new String[] {"unlock"});
                    }
                });

        nb.showNotification();
    }

    public static NotificationBuilder createNotificationBuilder() {
        NotificationBuilder nb = new NotificationBuilder();
        LightDefaultNotification style = new LightDefaultNotification();
        style.withCloseButton(new NullCloseButton());
        nb.withStyle(style);
        nb.withFadeInAnimation(true);
        nb.withFadeOutAnimation(true);
        nb.withPosition(Positions.NORTH_EAST);
        // nb.withDisplayTime(10000);

        return nb;
    }
}
