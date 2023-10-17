//g++ -std=c++11 -o myprogram IdentifyLiver.cpp $(pkg-config --cflags --libs opencv4)
#include <iostream>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/opencv.hpp>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

vector<vector<Point>> FindLiver(string LiverImagePath); //declared
Mat OverlayTexture(Mat OriginalImage, Mat cancerLiver, vector<vector<Point>> contoursHealthy); //declared

int main() {

    string pathHealthy = "./uncut_images_test/image2.png";
    string pathCancer = "./cancer.jpg";

    Mat cancerousImg = imread("./cancer.jpg");
    Mat healthyImg = imread("./uncut_images_test/image2.png");


    Scalar lowerThreshold = Scalar(59, 80, 166); // Example lower threshold in RGB
    Scalar upperThreshold = Scalar(170, 173, 220); 

    // Scalar lowerThreshold = Scalar(59, 90, 166); // Example lower threshold in RGB
    // Scalar upperThreshold = Scalar(100, 140, 200); 


    Mat mask1;
    inRange(healthyImg, lowerThreshold, upperThreshold, mask1);
    imwrite("maskAverage.png", mask1);

    vector<vector<Point>> contours = FindLiver("./maskAverage.png");

    vector<vector<Point>> contoursFiltered;


    // Output the shape of the contours array
    cout << "Number of contours: " << contours.size() << endl;
    int contoursMax = 0;
    for (size_t i = 0; i < contours.size(); ++i) {
        //find max 
        if (contours[i].size() > contoursMax) {
            contoursMax = contours[i].size();

        }

    }

   

    for (size_t i = 0; i < contours.size(); ++i) {

       float relativeVal = static_cast<float>(contours[i].size()) / contoursMax; // normalize
       cout << relativeVal << endl;
        

        if (relativeVal >= 0.1) {
            contoursFiltered.push_back(contours[i]);
        }
        

    }

    //create a filtered array 
    for (size_t i = 0; i < contoursFiltered.size()-4; ++i) {
        cout << "Number of points in contour " << i << ": " << contoursFiltered[i].size() << endl;
    

    }



    Mat finalImage = imread(pathHealthy); //save image for final overlay 

    resize(cancerousImg,cancerousImg,finalImage.size()); 

    
    drawContours(finalImage, contoursFiltered, -1, 255, 4 ); //finished detecting liver 



    Mat mask = Mat::zeros(finalImage.size(), CV_8U);
    vector<vector<Point>> contoursForMask = {contours};
    fillPoly(mask, contoursForMask, Scalar(255));
    Mat outputImage = Mat::zeros(finalImage.size(), finalImage.type());

    resize(mask,mask,finalImage.size());
    resize(outputImage,outputImage,finalImage.size());

    cancerousImg.copyTo(outputImage, mask);

    Mat Image = OverlayTexture(finalImage, outputImage,contours);

    imwrite("overlayyed.jpg",Image);
    

}

vector<vector<Point>> FindLiver(string LiverImagePath) {


    Mat image = imread(LiverImagePath); //save image

    //detect image of the liver, use greyscale, thresholding, edge detection
    Mat grescaleImg;
    cvtColor(image, grescaleImg, cv::COLOR_BGR2GRAY);    
    Mat thresholdimg;
    adaptiveThreshold(grescaleImg, thresholdimg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 2);



    //perform morphological opperations
    Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    morphologyEx(thresholdimg, thresholdimg, cv::MORPH_OPEN, kernel);
    morphologyEx(thresholdimg, thresholdimg, cv::MORPH_CLOSE, kernel);

    //perform cannedy edge detection 
    Canny(thresholdimg,thresholdimg,100,200,3,false);



    //using vector to find contours in the current image and save it, retr external for outer edge only, 
    vector<vector<Point>> contouredPoints;
    findContours(thresholdimg, contouredPoints, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    
    return contouredPoints;


}

Mat OverlayTexture(Mat OriginalImage, Mat cancerLiver, vector<vector<Point>> contoursHealthy) {

    // Read the cancer liver image and resize it to match the size of the healthy liver image
    resize(cancerLiver,cancerLiver,OriginalImage.size()); 

    //determine min max y, make the image more focused
    int minY = INT_MAX;
    int maxY = INT_MIN;

    Mat blended;
    addWeighted(cancerLiver, 0.6, OriginalImage, 1, 0.0, blended);
    return blended;


}