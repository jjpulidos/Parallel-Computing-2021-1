#include <iostream>
#include <stdlib.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

void checkNumArgs(int numArgs, char* progName){

    if(numArgs != 4){

        cout
            << "Usage:\n  "
            << progName << " ksize origin destination\n"
            << "    ksize: positive integer size of filter window\n"
            << "    origin: path of image to be smoothed\n"
            << "    destination: path to write result to"
            << endl;
        exit(1);
    }
}

int parseKsize(char *arg){

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
