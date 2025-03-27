package app.m0rf30.cieid.Firma;

import java.awt.Color;
import java.awt.Image;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

public class MoveablePicture extends JPanel implements MouseListener, MouseMotionListener {

    private int x, y;
    private String signFilePath;

    private void loadImage() {
        try {
            Image img;
            int width = this.getWidth();
            int height = this.getHeight();
            img = ImageIO.read(new File(signFilePath));
            JLabel imgLabel =
                    new JLabel(new ImageIcon(img.getScaledInstance(width, -1, Image.SCALE_SMOOTH)));
            imgLabel.setBounds(0, 0, width, height);
            imgLabel.setVerticalAlignment(SwingConstants.BOTTOM);
            this.add(imgLabel);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public void reloadImage() {
        this.removeAll();
        this.loadImage();
    }

    public MoveablePicture(String signFilePath) {
        // TODO Auto-generated constructor stub
        this.signFilePath = signFilePath;
        this.setLayout(null);
        this.setSize(50, 25);
        this.setBackground(new Color(255, 255, 255, 0));
        this.setBorder(BorderFactory.createDashedBorder(Color.black));
        this.loadImage();
        this.addMouseListener(this);
        this.addMouseMotionListener(this);
    }

    @Override
    public void mouseDragged(MouseEvent e) {
        // TODO Auto-generated method stub

        int tmpX = (e.getX() + e.getComponent().getX() - x);
        int tmpY = (e.getY() + e.getComponent().getY() - y);

        if (tmpX < 0) {
            tmpX = 0;
        }
        if (tmpY < 0) {
            tmpY = 0;
        }

        if (tmpX > this.getParent().getWidth() - this.getWidth()) {
            tmpX = this.getParent().getWidth() - this.getWidth();
        }
        if (tmpY > this.getParent().getHeight() - this.getHeight()) {
            tmpY = this.getParent().getHeight() - this.getHeight();
        }

        e.getComponent().setLocation(tmpX, tmpY);
        repaint();
    }

    @Override
    public void mouseMoved(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mouseClicked(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mouseEntered(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mouseExited(MouseEvent e) {
        // TODO Auto-generated method stub

    }

    @Override
    public void mousePressed(MouseEvent e) {
        // TODO Auto-generated method stub
        x = e.getX();
        y = e.getY();
    }

    @Override
    public void mouseReleased(MouseEvent e) {
        // TODO Auto-generated method stub

    }
}
