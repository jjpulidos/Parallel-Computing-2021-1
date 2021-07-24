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

Mat img_hsv, img, new_h, new_s, new_v, dst_h, dst_s, dst_v;
int total_threads;
int ksize;
vector<pair<int, int>> delta;

vector<uchar> medianFilterWindow(int i, int j) {

  // Median Calculations for a window of ksize x ksize

  vector<uchar> pixelH(ksize * ksize);
  vector<uchar> pixelS(ksize * ksize);
  vector<uchar> pixelV(ksize * ksize);

  for (int k = 0; k < ksize * ksize; ++k) {
    pixelH[k] = new_h.at<uchar>(i + delta[k].first, j + delta[k].second);
    pixelS[k] = new_s.at<uchar>(i + delta[k].first, j + delta[k].second);
    pixelV[k] = new_v.at<uchar>(i + delta[k].first, j + delta[k].second);
  }

  // Sorting those windows
  sort(pixelH.begin(), pixelH.end());
  sort(pixelS.begin(), pixelS.end());
  sort(pixelV.begin(), pixelV.end());

  // Getting the n/2 element (Median) after sorting for every HSV channel
  vector<uchar> pixels = {pixelH[(ksize * ksize) / 2],
                          pixelS[(ksize * ksize) / 2],
                          pixelV[(ksize * ksize) / 2]};
  return pixels;
}

void medianFilter(int thread_id) {

  //  Median Filter Function

  int n = new_h.rows / total_threads;
  int start = n * thread_id - ksize;
  int end = start + n;
  start = start < 0 ? 0 : start;

  // Vector of pixels for every median window calculations for HSV channels
  vector<uchar> pixels = {new_h.at<uchar>(1, 1), new_s.at<uchar>(1, 1),
                          new_v.at<uchar>(1, 1)};

  for (int i = start; i < end; i++) {

    for (int j = 0; j < new_h.cols - ksize - 1; j++) {

      pixels = medianFilterWindow(i, j);

      dst_h.at<uchar>(i, j) = pixels[0];
      dst_s.at<uchar>(i, j) = pixels[1];
      dst_v.at<uchar>(i, j) = pixels[2];
    }
  }
}

int main(int argc, char *argv[]) {

  // Getting Arguments from execution command
  checkNumArgs(5, argc, argv[0]);

  // Parsing Arguments
  ksize = parsePosInt(argv[1]);
  total_threads = parsePosInt(argv[2]);
  img = getImg(argv[3]);

  // Converting BGR default color mode to HSV in order to apply Median Filter
  // Correctly Median Filter shouldnt be applied in RGB or BGR color modes
  // because are not going to get the real Median between all the channels, If
  // you are using a GrayScale Image is ok, but with Color Images you should do
  // a HSV conversion
  cvtColor(img, img_hsv, COLOR_BGR2HSV);
  vector<Mat> hsvChannels(3);
  split(img_hsv, hsvChannels);

  // Delta is a vector of the offset for the windowing, and depends of ksize
  for (int i = 0; i < ksize; i++) {
    for (int j = 0; j < ksize; j++) {
      delta.push_back(make_pair(i, j));
    }
  }

  new_h = hsvChannels[0];
  new_s = hsvChannels[1];
  new_v = hsvChannels[2];

  // Instead of using malloc/free we drawn on C++ Memory Management and
  // functions like "clone" in OpenCV  library
  dst_h = new_h.clone();
  dst_s = new_s.clone();
  dst_v = new_v.clone();

  // Start timing
  auto start = high_resolution_clock::now();

// parallel processing
#pragma omp parallel for
  for (int i = 0; i < total_threads; i++) {

    medianFilter(i);
  }

  // End timing
  auto end = high_resolution_clock::now();
  duration<double, milli> total_time = (end - start);
  /* cout << "time    = " << total_time.count() / 1000 << '\n'; */
  cout << total_time.count() / 1000 << '\n';

  // After applied MedianFilter for each channel, were merged due to build the
  // Image Again
  /*   vector<Mat> channels = {dst_h, dst_s, dst_v}; */
  /*   Mat merged, filtered; */
  /*   merge(channels, merged); */
  /*  */
  /*   // Changing HSV mode color to BGR again */
  /*   cvtColor(merged, filtered, COLOR_HSV2BGR); */
  /*  */
  /*   // Saving Image results */
  /*   putImg(filtered, argv[4]); */
}
