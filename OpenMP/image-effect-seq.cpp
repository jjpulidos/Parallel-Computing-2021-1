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

uchar medianFilterWindow(const cv::Mat &src, int i, int j) {

  // Median Calculations for a window of ksize x ksize

  vector<uchar> pixel(ksize * ksize);
  for (int k = 0; k < ksize * ksize; ++k) {
    pixel[k] = src.at<uchar>(i + delta[k].first, j + delta[k].second);
  }

  // Sorting window values and returning Median Element
  sort(pixel.begin(), pixel.end());

  return pixel[(ksize * ksize) / 2];
}

Mat medianFilter(const cv::Mat &src) {

  // Median Filter Function
  dst = src.clone();

  // For every window of ksize * ksize in original Image was MedianFiltering
  for (int i = 1; i < src.rows - ksize; ++i) {
    for (int j = 1; j < src.cols - ksize; ++j) {
      dst.at<uchar>(i, j) = medianFilterWindow(src, i, j);
    }
  }

  return dst;
}

int main(int argc, char *argv[]) {

  // Getting Arguments from execution command
  checkNumArgs(4, argc, argv[0]);

  // Parsing Arguments
  ksize = parsePosInt(argv[1]);
  img = getImg(argv[2]);

  // Converting BGR default color mode to HSV in order to apply Median Filter
  // Correctly Median Filter shouldnt be applied in RGB or BGR color modes
  // because are not going to get the real Median between all the channels, If
  // you are using a GrayScale Image is ok, but with Color Images you should do
  // a HSV conversion
  cvtColor(img, img_hsv, COLOR_BGR2HSV);
  vector<Mat> hsvChannels(3);
  split(img_hsv, hsvChannels);

  // Delta is a vector of the offset for the windowing, and depends of ksize
  for (int i = -1; i < ksize - 1; i++) {
    for (int j = -1; j < ksize - 1; j++) {
      delta.push_back(make_pair(i, j));
    }
  }

  // Start timing
  auto start = high_resolution_clock::now();

  new_h = medianFilter(hsvChannels[0]);
  new_s = medianFilter(hsvChannels[1]);
  new_v = medianFilter(hsvChannels[2]);

  // End timing
  auto end = high_resolution_clock::now();
  duration<double, milli> total_time = (end - start);
  /* cout << "time    = " << total_time.count() / 1000 << '\n'; */
  cout << total_time.count() / 1000 << '\n';

  // After applied MedianFilter for each channel, were merged due to build the
  // Image Again
  /*   vector<Mat> channels = {new_h, new_s, new_v}; */
  /*   Mat merged, filtered; */
  /*   merge(channels, merged); */
  /*  */
  /*   // Changing HSV mode color to BGR again */
  /*   cvtColor(merged, filtered, COLOR_HSV2BGR); */
  /*  */
  /*   // Saving Image results */
  /*   putImg(filtered, argv[3]); */
}
