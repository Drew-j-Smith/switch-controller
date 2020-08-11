import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.highgui.HighGui;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.imageio.ImageIO;
import javax.swing.text.MutableAttributeSet;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.DataBufferInt;
import java.io.File;
import java.io.IOException;

public class OpenCVutil {
    public OpenCVutil(){}

    private static Robot robot;

    static {
        try {
            robot = new Robot();
        } catch (AWTException e) {
            e.printStackTrace();
        }
    }

    public static BufferedImage screenshot(){
        //return screenshot(new Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));
        return screenshot(new Rectangle(1920,1080));
    }

    public static BufferedImage screenshot(Rectangle screenRect) {
        BufferedImage capture = robot.createScreenCapture(screenRect);
        BufferedImage img = new BufferedImage(capture.getWidth(), capture.getHeight(), BufferedImage.TYPE_3BYTE_BGR);
        img.setRGB(0, 0, capture.getWidth(), capture.getHeight(), ((DataBufferInt) capture.getRaster().getDataBuffer()).getData(), 0, capture.getWidth());
        return img;
    }

    public static void saveImage(BufferedImage image, String fileName){
        try {
            ImageIO.write(image, "bmp", new File(fileName + ".bmp"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static Mat retrieveImage(String fileName){
        return Imgcodecs.imread(fileName, Imgcodecs.IMREAD_COLOR);
    }

    public static Mat bufferedImgToMat(BufferedImage image){
        Mat mat = new Mat(image.getHeight(), image.getWidth(), CvType.CV_8UC3);
        byte[] imageInBytes = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
        mat.put(0, 0, imageInBytes);
        return mat;
    }

    public static BufferedImage matToBufferedImage(Mat mat){
        return (BufferedImage)(HighGui.toBufferedImage(mat));
    }

    public static Mat matchTemplate(Mat source, Mat template){
        return matchTemplate(source, template, Imgproc.TM_CCOEFF_NORMED);
    }

    public static Mat matchTemplate(Mat source, Mat template, int matchType){
        Mat result = new Mat();
        int result_cols = source.cols() - template.cols() + 1;
        int result_rows = source.rows() - template.rows() + 1;
        result.create(result_rows, result_cols, CvType.CV_32FC1);

        Imgproc.matchTemplate(source, template, result, matchType);
        return result;
    }


    public static Core.MinMaxLocResult findMatchLocation(Mat mat){
        return Core.minMaxLoc(mat);
    }

}
