package it.ipzs.cieid.Firma;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import javax.swing.*;
import org.apache.pdfbox.Loader;
import org.apache.pdfbox.io.RandomAccessReadBufferedFile;
import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.rendering.PDFRenderer;

public class PdfPreview {
    private final JPanel prPanel;
    private final String filePath;
    private final String signImagePath;
    private int pdfPageIndex;
    private int pdfNumPages;
    private List<BufferedImage> renderedPages = new ArrayList<BufferedImage>();
    private final JLabel imgLabel;
    private final ImageIcon imgIcon;
    private final MoveablePicture signImage;
    private final JPanel imgPanel;

    public PdfPreview(JPanel panelPdfPreview, String pdfFilePath, String signImagePath) {
        this.prPanel = panelPdfPreview;
        this.filePath = pdfFilePath;
        this.signImagePath = signImagePath;
        this.pdfPageIndex = 0;
        imgIcon = new ImageIcon();
        imgLabel = new JLabel();
        imgPanel = new JPanel();
        imgPanel.setLayout(new BorderLayout(0, 0));
        imgPanel.setBackground(Color.white);
        signImage = new MoveablePicture(signImagePath);
        imgPanel.add(signImage);
        imgPanel.add(imgLabel);

        try {
            PDDocument document = Loader.loadPDF(new RandomAccessReadBufferedFile(filePath));
            pdfNumPages = document.getNumberOfPages();
            System.out.println("Pdf page: " + pdfNumPages);
            PDFRenderer renderer = new PDFRenderer(document);

            prPanel.removeAll();

            for (pdfPageIndex = 0; pdfPageIndex < pdfNumPages; pdfPageIndex++) {
                BufferedImage renderedPage = renderer.renderImage(pdfPageIndex);
                renderedPages.add(renderedPage);
            }

            pdfPageIndex = 0;
            showPreview();
            document.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            System.out.println("PDF File not found");
            e.printStackTrace();
        }
    }

    private void showPreview() {
        Image tmpImg = renderedPages.get(pdfPageIndex);

        int width = prPanel.getWidth();
        int height = prPanel.getHeight();

        int tmpImgWidth = tmpImg.getWidth(null);
        int tmpImgHeight = tmpImg.getHeight(null);

        int imgHeight = height;
        int imgWidth = width;

        if (tmpImgWidth > tmpImgHeight) {
            imgHeight = (width * tmpImgHeight) / tmpImgWidth;

            if (imgHeight > height) {
                imgWidth = (height * tmpImgWidth) / tmpImgHeight;
                imgHeight = (imgWidth * tmpImgHeight) / tmpImgWidth;
            }
        } else {
            imgWidth = (height * tmpImgWidth) / tmpImgHeight;

            if (imgWidth > width) {
                imgHeight = (width * imgHeight) / tmpImgWidth;
                imgWidth = (imgHeight * tmpImgWidth) / imgHeight;
            }
        }

        imgIcon.setImage(tmpImg.getScaledInstance(imgWidth, imgHeight, Image.SCALE_AREA_AVERAGING));
        imgLabel.setIcon(imgIcon);
        imgLabel.setHorizontalAlignment(JLabel.CENTER);
        imgLabel.setVerticalAlignment(JLabel.CENTER);
        imgLabel.revalidate();
        imgLabel.repaint();

        imgPanel.setMaximumSize(new Dimension(imgWidth, imgHeight));
        imgPanel.updateUI();

        prPanel.removeAll();
        prPanel.add(imgPanel);
        prPanel.updateUI();
    }

    public void prevImage() {
        if ((pdfPageIndex - 1) >= 0) {
            pdfPageIndex -= 1;
        }

        showPreview();
    }

    public void nextImage() {
        if ((pdfPageIndex + 1) < pdfNumPages) {
            pdfPageIndex += 1;
        }

        showPreview();
    }

    public int getSelectedPage() {
        return pdfPageIndex;
    }

    public float[] signImageInfos() {
        float[] infos = new float[4];

        float x = ((float) signImage.getX() / (float) imgPanel.getWidth());
        float y =
                ((float) (signImage.getY() + signImage.getHeight()) / (float) imgPanel.getHeight());
        float w = ((float) signImage.getWidth() / (float) imgPanel.getWidth());
        float h = ((float) signImage.getHeight() / (float) imgPanel.getHeight());

        infos[0] = x;
        infos[1] = y;
        infos[2] = w;
        infos[3] = h;

        return infos;
    }
}
