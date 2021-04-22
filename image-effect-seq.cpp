#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "input.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

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

Mat medianFilter(const cv::Mat &src){

    dst = src.clone();

    for (int i = 1; i < src.rows - ksize; ++i){
        for (int j = 1; j < src.cols - ksize; ++j){
            dst.at<uchar>(i, j) = medianFilterWindow(src, i, j);
        }
    }

    return dst;
}


int main (int argc, char* argv[]) {

    checkNumArgs(4, argc, argv[0]);
    ksize = parsePosInt(argv[1]);

    img = getImg(argv[2]);
    cvtColor(img, img_hsv, COLOR_BGR2HSV);
    vector<Mat> hsvChannels(3);
    split(img_hsv, hsvChannels);

    for(int i = -1; i < ksize - 1; i++){
        for(int j = -1; j < ksize - 1; j++){
            delta.push_back(make_pair(i, j));
        }
    }

    auto start = high_resolution_clock::now();
    new_h = medianFilter(hsvChannels[0]);
    new_s = medianFilter(hsvChannels[1]);
    new_v = medianFilter(hsvChannels[2]);
    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);
    cout << "time    = " << total_time.count() / 1000 << '\n';

    vector<Mat> channels = {new_h, new_s, new_v};
    Mat merged, filtered;
    merge(channels, merged);
    cvtColor(merged, filtered, COLOR_HSV2BGR);
    putImg(filtered, argv[3]);
}
