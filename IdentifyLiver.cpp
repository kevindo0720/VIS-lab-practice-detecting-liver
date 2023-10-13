#include <iostream>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/opencv.hpp>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

vector<vector<Point>> FindLiver(string LiverImagePath); //declared
Mat OverlayTexture(Mat OriginalImage, Mat cancerLiver, vector<vector<Point>> contoursHealthy); //declared

int main() {

    string pathHealthy = "./healthy.jpg";
    string pathCancer = "./cancer.jpg";

    Mat cancerousImg = imread("./cancer.jpg");

    


    vector<vector<Point>> contours = FindLiver(pathHealthy);
    Mat finalImage = imread(pathHealthy); //save image for final overlay 

    resize(cancerousImg,cancerousImg,finalImage.size()); 
    
    drawContours(finalImage, contours, -1, 255, 4 ); //finished detecting liver 



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