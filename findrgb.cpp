//g++ -std=c++11 -o myprogram IdentifyLiver.cpp $(pkg-config --cflags --libs opencv4)
#include <iostream>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/opencv.hpp>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


int main() {

    

    Mat image1 = imread("./uncut_images_test/image4.png");
    Mat image2 = imread("./uncut_images_test/image5.png");
    Mat image7= imread("./uncut_images_test/image8.png");


    int minRed = 255, minGreen = 255, minBlue = 255;
    int maxRed = 0, maxGreen = 0, maxBlue = 0;

    // Iterate through each pixel and find minimum and maximum RGB values
    for (int y = 0; y < image1.rows; ++y) {
        for (int x = 0; x < image1.cols; ++x) {
            Vec3b pixel = image1.at<Vec3b>(y, x);
            int blue = pixel[0];
            int green = pixel[1];
            int red = pixel[2];

            // cout << red << " " << green << " " << blue << endl;

            // Update minimum values
            minBlue = min(minBlue, blue);
            minGreen = min(minGreen, green);
            minRed = min(minRed, red);

            // Update maximum values
            maxBlue = max(maxBlue, blue);
            maxGreen = max(maxGreen, green);
            maxRed = max(maxRed, red);
        }
    }

    cout << minRed << " " << minGreen << " " << minBlue << endl;
    cout << maxRed << " " << maxGreen << " " << maxBlue << endl;
    minRed = 255, minGreen = 255, minBlue = 255;
    maxRed = 0, maxGreen = 0, maxBlue = 0;



   // Iterate through each pixel and find minimum and maximum RGB values
    for (int y = 0; y < image2.rows; ++y) {
        for (int x = 0; x < image2.cols; ++x) {
            Vec3b pixel = image1.at<Vec3b>(y, x);
            int blue = pixel[0];
            int green = pixel[1];
            int red = pixel[2];


            // Update minimum values
            minBlue = min(minBlue, blue);
            minGreen = min(minGreen, green);
            minRed = min(minRed, red);

            // Update maximum values
            maxBlue = max(maxBlue, blue);
            maxGreen = max(maxGreen, green);
            maxRed = max(maxRed, red);
        }
    }


    cout << minRed << " " << minGreen << " " << minBlue << endl;
    cout << maxRed << " " << maxGreen << " " << maxBlue << endl;
    minRed = 255, minGreen = 255, minBlue = 255;
    maxRed = 0, maxGreen = 0, maxBlue = 0;


    for (int y = 0; y < image7.rows; ++y) {
        for (int x = 0; x < image7.cols; ++x) {
            Vec3b pixel = image1.at<Vec3b>(y, x);
            int blue = pixel[0];
            int green = pixel[1];
            int red = pixel[2];

            cout << red << " " << green << " " << blue << endl;

            // Update minimum values
            minBlue = min(minBlue, blue);
            minGreen = min(minGreen, green);
            minRed = min(minRed, red);

            // Update maximum values
            maxBlue = max(maxBlue, blue);
            maxGreen = max(maxGreen, green);
            maxRed = max(maxRed, red);
        }
    }

    cout << minRed << " " << minGreen << " " << minBlue << endl;
    cout << maxRed << " " << maxGreen << " " << maxBlue << endl;
    minRed = 255, minGreen = 255, minBlue = 255;
    maxRed = 0, maxGreen = 0, maxBlue = 0;




}
