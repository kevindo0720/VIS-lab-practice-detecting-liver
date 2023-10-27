//g++ -std=c++11 -o myprogram IdentifyLiver.cpp $(pkg-config --cflags --libs opencv4)
#include <iostream>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/opencv.hpp>
#include </opt/homebrew/Cellar/opencv/4.8.1_1/include/opencv4/opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

//helpers
vector<vector<Point>> FindLiver(string LiverImagePath); //declared
Mat OverlayTexture(Mat OriginalImage, Mat cancerLiver, vector<vector<Point>> contoursHealthy); //declared
int HuMomentScore(string liver_shape_path, vector<vector<Point>> filteredContours); //declared
Mat DrawContoursOnLiver(Mat frame); //declared

#include <opencv2/opencv.hpp>

int main() {
    // Open a video file for reading
    cv::VideoCapture cap("./input_video.mp4");

    // Check if the video file opened successfully
    if (!cap.isOpened()) {
        cerr << "Error: Could not open the video file." << endl;
        return -1;
    }

    // Get video frame properties
    int frameWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
    double framesPerSecond = cap.get(CAP_PROP_FPS);

    // Define the codec and create a VideoWriter object
    VideoWriter videoWriter("output_video.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), framesPerSecond, Size(frameWidth, frameHeight));

    // Loop through the video frames
    Mat frame;
    while (true) {
        // Read a frame from the video
        cap >> frame;

        // Break the loop if there are no more frames
        if (frame.empty()) {
            break;
        }

        // Your image processing code goes here
        // For example, convert the frame to grayscale
        Mat processedFrame;
        
        processedFrame = DrawContoursOnLiver(frame);

        // Write the processed frame to the output video
        videoWriter.write(processedFrame);

        // Display the processed frame (optional, for visualization)
        imshow("Processed Frame", processedFrame);

        // Press 'q' to exit the loop
        if (waitKey(1) == 'q') {
            break;
        }
    }

    // Release the video capture and writer objects
    cap.release();
    videoWriter.release();

    // Close all windows
    destroyAllWindows();

    return 0;
}


Mat DrawContoursOnLiver(Mat frame) {

    string pathHealthy = "./uncut_images_test/image12.png";
    string pathCancer = "./cancer(3).png";

    Mat cancerousImg = imread("./uncut_images_test/cancer2.png");
    // Mat healthyImg = imread("./uncut_images_test/image12.png");


    // for (int y = 0; y < cancerousImg.rows; ++y) {
    //     for (int x = 0; x < cancerousImg.cols; ++x) {
    //         cv::Vec4b &pixel = cancerousImg.at<cv::Vec4b>(y, x);
    //         if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
    //             pixel[3] = 0; // Set alpha channel to 0 (fully transparent) for white pixels
    //         }
    //     }
    // }
    Mat healthyImg = frame;


    Scalar lowerThreshold = Scalar(90, 120, 200); // Example lower threshold in RGB
    Scalar upperThreshold = Scalar(120, 210, 255); 


    
    //150-200 for 1st col rules out the liver

    // Scalar lowerThreshold = Scalar(59, 90, 166); // Example lower threshold in RGB
    // Scalar upperThreshold = Scalar(100, 105, 200); 


    Mat mask1;
    inRange(healthyImg, lowerThreshold, upperThreshold, mask1);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
    morphologyEx(mask1, mask1, MORPH_CLOSE, kernel);  // Closing operation
    morphologyEx(mask1, mask1, MORPH_OPEN, kernel);
    imwrite("maskAverage.png", mask1);


    vector<vector<Point>> contours = FindLiver("./maskAverage.png");
    

    // Output the shape of the contours array
    cout << "Number of contours: " << contours.size() << endl;
    int contoursMax = 0;
    for (size_t i = 0; i < contours.size(); ++i) {
        //find max 
        if (contours[i].size() > contoursMax) {
            contoursMax = contours[i].size();

        }

    }

    vector<vector<Point>> contoursFiltered; //to save only big contours in an image
    for (size_t i = 0; i < contours.size(); ++i) {

       float relativeVal = static_cast<float>(contours[i].size()) / contoursMax; // normalize
       cout << relativeVal << endl;
        
        if (relativeVal > 0.1 ) {
            contoursFiltered.push_back(contours[i]);
        }        

    }

    
    //check the results of the filtered contour array
    for (size_t i = 0; i < contoursFiltered.size(); ++i) {
        cout << "Number of points in contour " << i << ": " << contoursFiltered[i].size() << endl;

    }

    

    int index = HuMomentScore("./healthy.jpg",contoursFiltered);


    vector<vector<Point>> contoursFinal;
    contoursFinal.push_back(contoursFiltered[index]);



    // Mat finalImage = imread(pathHealthy); //save image for final overlay 
    Mat finalImage = frame;


    cout << "contoursize: " << contoursFinal[0].size() << ", image size: " << finalImage.size() << endl; 
    // Check if there are at least two elements in contoursFiltered
    // drawContours(finalImage, contoursFinal, -1, 255, 2 ); //finished detecting liver 


    // vector<vector<Point>> contoursToPass;

    // Mat mask = Mat::zeros(finalImage.size(), CV_8U);
    // vector<vector<Point>> contoursForMask = {contoursToPass};
    // fillPoly(mask, contoursForMask, Scalar(255));
    // Mat outputImage = Mat::zeros(finalImage.size(), finalImage.type());

    // resize(mask,mask,finalImage.size());
    // resize(outputImage,outputImage,finalImage.size());

    // cancerousImg.copyTo(outputImage, mask);

    Mat Image = OverlayTexture(finalImage, cancerousImg,contoursFinal);

    // imwrite("overlayyed.jpg",Image);

    return Image;
    

}

vector<vector<Point>> FindLiver(string LiverImagePath) {


    Mat image = imread(LiverImagePath); //save image

    //detect image of the liver, use greyscale, thresholding, edge detection
    Mat grescaleImg;
    cvtColor(image, grescaleImg, cv::COLOR_BGR2GRAY);    
    Mat thresholdimg;
    adaptiveThreshold(grescaleImg, thresholdimg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 2);



    //perform morphological opperations
    Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    morphologyEx(thresholdimg, thresholdimg, cv::MORPH_OPEN, kernel);
    morphologyEx(thresholdimg, thresholdimg, cv::MORPH_CLOSE, kernel);

    //perform cannedy edge detection 
    Canny(thresholdimg,thresholdimg,100,200,5,false);



    //using vector to find contours in the current image and save it, retr external for outer edge only, 
    vector<vector<Point>> contouredPoints;
    findContours(thresholdimg, contouredPoints, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    
    return contouredPoints;


}

// Mat OverlayTexture(Mat OriginalImage, Mat cancerLiver, vector<vector<Point>> contoursHealthy) {
//     Mat mask = Mat::zeros(OriginalImage.size(), CV_8U);

//     // Create a mask for the contour
//     drawContours(mask, contoursHealthy, 0, Scalar(255), FILLED);

//     // Determine the bounding rectangle of the contour
//     Rect boundingRect1 = boundingRect(contoursHealthy[0]);

//     // Extract the region of interest (ROI) from the cancerLiver image using the contour mask
//     Mat cancerLiverROI = cancerLiver(boundingRect1).clone();
//     cancerLiverROI.setTo(Scalar(0, 0, 0, 0), ~mask(boundingRect1)); // Set area outside the contour to transparent

//     // Overlay the cancerLiverROI onto the OriginalImage using the contour mask
//     cancerLiverROI.copyTo(OriginalImage(boundingRect1), mask(boundingRect1));

//     return OriginalImage;
// }


Mat OverlayTexture(Mat OriginalImage, Mat cancerLiver, vector<vector<Point>> contoursHealthy) {

    // Read the cancer liver image and resize it to match the size of the healthy liver image
    

    int minY = INT_MAX;
    int maxY = INT_MIN;
    int minX = INT_MAX;
    int maxX = INT_MIN;


    for(int i = 0; i< contoursHealthy[0].size(); i++) {

        if (contoursHealthy[0][i].x > maxX) {
            
            maxX = contoursHealthy[0][i].x;


        } else if (contoursHealthy[0][i].x < minX) {

            minX = contoursHealthy[0][i].x;
        }


        if (contoursHealthy[0][i].y > maxY) {
            
            maxY = contoursHealthy[0][i].y;


        } else if (contoursHealthy[0][i].y < minY) {

            minY = contoursHealthy[0][i].y;
        }


        
    }

    double liverSize = (maxX - minX) * (maxY - minY);

    double originalSize = OriginalImage.cols * OriginalImage.rows;


    cout << "size of liver: " << liverSize << ", ratio: " <<  liverSize / originalSize << endl;
    //determine min max y, make the image more focused

    Size newSize((maxX - minX-30), (maxY - minY ));
    resize(cancerLiver,cancerLiver,newSize); 


    Mat masking;

    // extractChannel(cancerLiver, masking, 3);

    double alpha = 0.7; // Weight for the overlay image (transparency factor)
    double beta = 1.0 - alpha; // Weight for the background image
    cv::addWeighted(OriginalImage(Rect(minX, minY, cancerLiver.cols, cancerLiver.rows)), alpha, cancerLiver, beta, 0.0, OriginalImage(Rect(minX, minY, cancerLiver.cols, cancerLiver.rows)));


    return OriginalImage;


}


int HuMomentScore(string liver_shape_path, vector<vector<Point>> filteredContours) {

    // Calculate moments for the liver shape (reference)
    Mat liver_shape = imread(liver_shape_path, IMREAD_GRAYSCALE);
    Moments moments_liver = moments(liver_shape);
    Mat hu_moments_liver;
    HuMoments(moments_liver, hu_moments_liver);


    double maxscore = 0;
    int maxindex = 0;
    int loopingindex = 0;
    int holderindex = 0;

    for (const auto& contour : filteredContours) {
        // Calculate moments for the current contour
        Moments moments_contour = moments(contour);
        Mat hu_moments_contour;
        HuMoments(moments_contour, hu_moments_contour);

        // Compare Hu Moments (you can define a threshold for matching)
        double matching_score = matchShapes(hu_moments_liver, hu_moments_contour, CONTOURS_MATCH_I1, 0.0);

        
        //threshold is 200
        if (matching_score - maxscore <=100 || maxscore - matching_score<=100) {

            holderindex = loopingindex;

        }

        else if (matching_score > maxscore) {
            

            maxindex = loopingindex;
            maxscore = matching_score;
        }
        cout << "Matching Score: " << matching_score << endl;
        loopingindex+=1;
    }

    if (holderindex > maxindex) {

        maxindex = holderindex;
    }

    return maxindex;

   
}