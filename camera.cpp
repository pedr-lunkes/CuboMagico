#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;


struct HSVRange {
    Scalar lower;
    Scalar upper;
};

HSVRange redRange1 = { Scalar(0, 100, 100), Scalar(10, 255, 255) };
HSVRange redRange2 = { Scalar(170, 100, 100), Scalar(179, 255, 255) };
HSVRange greenRange = { Scalar(35, 100, 100), Scalar(75, 255, 255) };
HSVRange blueRange = { Scalar(105, 100, 100), Scalar(130, 255, 255) };
HSVRange yellowRange = { Scalar(20, 100, 100), Scalar(35, 255, 255) };
HSVRange orangeRange = { Scalar(7, 100, 100), Scalar(25, 255, 255) };


void superSaturateImage(Mat& image) {
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            Vec3b& pixel = image.at<Vec3b>(y, x);
            uchar r = pixel[2];
            uchar g = pixel[1];
            uchar b = pixel[0];

            uchar maxVal = max(max(r, g), b);
            double factor = 255.0 / maxVal;

            if (maxVal != 0) {
                pixel[2] = saturate_cast<uchar>(r * factor);
                pixel[1] = saturate_cast<uchar>(g * factor);
                pixel[0] = saturate_cast<uchar>(b * factor);
            }
        }
    }
}

int main() {
    VideoCapture cap(0);

    if (!cap.isOpened()) {
        cerr << "Error: Could not open the camera." << endl;
        return -1;
    }

    string windowName = "Camera Feed";
    namedWindow(windowName);

    int frameCount = 0;

    while(true){
         Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cerr << "Error: Captured frame is empty." << endl;
            break;
        }

        imshow(windowName, frame);

        char start = waitKey(30);
        if(start == 's'){
            cout << "Start recording" << endl;
            break;
        }
    }

    while (true) {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            cerr << "Error: Captured frame is empty." << endl;
            break;
        }

        imshow(windowName, frame);

        if (frameCount % 10 == 0) { // Save a frame every 30 frames         
            superSaturateImage(frame);

            Mat hsv;
            cvtColor(frame, hsv, COLOR_BGR2HSV);

            // Create masks for the Rubik's Cube colors
            Mat maskRed1, maskRed2, maskGreen, maskBlue, maskYellow, maskOrange, maskWhite;

            inRange(hsv, redRange1.lower, redRange1.upper, maskRed1);
            inRange(hsv, redRange2.lower, redRange2.upper, maskRed2);
            inRange(hsv, greenRange.lower, greenRange.upper, maskGreen);
            inRange(hsv, blueRange.lower, blueRange.upper, maskBlue);
            inRange(hsv, yellowRange.lower, yellowRange.upper, maskYellow);
            inRange(hsv, orangeRange.lower, orangeRange.upper, maskOrange);
            inRange(hsv, Scalar(0, 0, 200), Scalar(179, 50, 255), maskWhite);

            // Combine all masks
            Mat mask = maskRed1 | maskRed2 | maskGreen | maskBlue | maskYellow | maskOrange | maskWhite;

            imshow("mask", mask);

            Mat newframe;
            bitwise_and(frame, frame, newframe, mask);

            Size size(28, 28);
            resize(newframe, newframe, size, 0, 0, INTER_NEAREST);

            string filename = "1/0-" + to_string(frameCount / 10) + ".png";
            imwrite(filename, newframe);
            cout << "Saved " << filename << endl;
        }

        frameCount++;

        // Wait for 30 ms and break the loop if the user presses the 'q' key
        char c = (char)waitKey(10);
        if (c == 'q' || c == 27) {
            break;
        }
    }

    // Release the camera
    cap.release();

    // Destroy all OpenCV windows
    destroyAllWindows();

    return 0;
}
