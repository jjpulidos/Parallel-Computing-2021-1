#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace cv;
using namespace chrono;

int parseKsize(char *arg);
Mat getImg(char *filename);
void putImg(Mat image, char *filename);

Mat img_hsv, img, new_h, new_s, new_v, dst;
vector<pair<int, int>> delta;
int ksize = 4;


uchar medianFilterWindow(const cv::Mat &src, int i, int j){

    vector<uchar> pixel(ksize * ksize);
    for (int k = 0; k < ksize * ksize; ++k){
        pixel[k] = src.at<uchar>(i+delta[k].first, j+ delta[k].second);
    }
    sort(pixel.begin(), pixel.end());
    return pixel[(ksize * ksize) / 2];

}

Mat medianFilter(const cv::Mat &src)
{
    dst = src.clone();

    for (int i = 1; i < src.rows - ksize; ++i){
        for (int j = 1; j < src.cols - ksize; ++j){
            dst.at<uchar>(i, j) = medianFilterWindow(src, i, j);
        }
    }

    return dst;
}




int main (int argc, char* argv[]) {

    if(argc != 4){
    
        cout
            << "Usage:\n  " 
            << argv[0] << " ksize origin destination\n"
            << "    ksize: positive integer size of filter window\n"
            << "    origin: path of image to be smoothed\n"
            << "    destination: path to write result to"
            << endl;
        return 1;
    }
    
    ksize = parseKsize(argv[1]);

    img = getImg(argv[2]);
    cvtColor(img, img_hsv, COLOR_BGR2HSV);
    vector<Mat> hsvChannels(3);
    split(img_hsv, hsvChannels);

    for(int i = -1; i<ksize-1; i++){
        for(int j = -1; j<ksize-1; j++){
            delta.push_back(make_pair(i, j));
        }
    }

    auto start = high_resolution_clock::now();
    new_h = medianFilter(hsvChannels[0]);
    new_s = medianFilter(hsvChannels[1]);
    new_v = medianFilter(hsvChannels[2]);
    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);
    cout << "Tiempo de respuesta: " << total_time.count() / 1000 << '\n';
    vector<Mat> channels = {new_h, new_s, new_v};
    Mat merged, filtered;
    merge(channels, merged);
    cvtColor(merged, filtered, COLOR_HSV2BGR);
    putImg(filtered, argv[3]);
    //imshow("windowsFilter", filtered);
    //waitKey(0);
    //destroyAllWindows();
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


