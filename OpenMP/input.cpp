#include <iostream>
#include <stdlib.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

void checkNumArgs(int targNum, int realNum, char* usageMessage){

    // Check the Number of Arguments

    if(realNum != targNum){

        cout
            << "Usage:\n  "
            << usageMessage
            << endl;
        exit(1);
    }
}

int parsePosInt(char *arg){

    // Parsing from String to Integer

    int size;
    try{

        size = stoi(string(arg));
        if(size < 1){

            throw invalid_argument("");
        }
    }
    catch(...){

        cout
            << "Invalid value for ksize (kernel window size)" 
            << endl;
        exit(3);
    }
    return size;
}

Mat getImg(char *filename){

    // Get the Image Using OpenCV, returns a  cv::Mat
    Mat image = imread(filename, IMREAD_COLOR); // Load an image
    if(image.empty()){

        cout
            << "Could not read origin image at " << filename
            << endl;
        exit(2);
    }
    return image;
}

void putImg(Mat image, char *filename){

    // Write a image in a png file using compression in the path specified in filename variable

    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    if(!imwrite(filename, image, compression_params)){

        cout
            << "Could not write destination image at " << filename
            << endl;
        exit(4);
    }
}
