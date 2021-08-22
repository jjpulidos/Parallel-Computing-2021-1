#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string>

using namespace std;
using namespace cv;

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
Mat runMedianFilte(const cv::Mat &img) {

  cvtColor(img, img_hsv, COLOR_BGR2HSV);
  vector<Mat> hsvChannels(3);
  split(img_hsv, hsvChannels);

  for (int i = -1; i < ksize - 1; i++) {
    for (int j = -1; j < ksize - 1; j++) {
      delta.push_back(make_pair(i, j));
    }
  }

  new_h = medianFilter(hsvChannels[0]);
  new_s = medianFilter(hsvChannels[1]);
  new_v = medianFilter(hsvChannels[2]);

  vector<Mat> channels = {new_h, new_s, new_v};
  Mat merged, filtered;
  merge(channels, merged);

  cvtColor(merged, filtered, COLOR_HSV2BGR);
  return filtered;
}

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int process_id, num_procs;
  MPI_Status status;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

  Mat image;

  image = imread("in/sana_noise720p.jpg", IMREAD_COLOR);
  if (image.empty()) {
    cout << "Could not open or find the image" << std::endl;
    return -1;
  }

  // get the image data
  int height = image.rows;
  int width = image.cols;
  int size = (height * width * 3) / (num_procs) + 1;

  printf("Processing a %dx%d image\n", height, width);

  cv ::Size smallSize(width / num_procs, height);

  std ::vector<float *> smallImages;

  cv ::Rect rect = cv ::Rect(process_id * smallSize.width, 0, smallSize.width,
                             smallSize.height);
  Mat img = cv ::Mat(image, rect);
  Mat imgFiltered = runMedianFilte(img);
  uchar *imgFilteredData;

  if (process_id != 0) {
    MPI_Send(imgFiltered.data, size, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
    /* cv::Mat combined; */
  } else {
    for (int j = 1; j < num_procs; j++) {
      // printf("Ready to recive from process %d\n", j);
      cv::Mat imgrecive;
      MPI_Recv(imgFilteredData, size, MPI_UNSIGNED_CHAR, j, 0, MPI_COMM_WORLD,
               &status);
      printf("Recived data from process %d\n", j);
      /* imwrite("prueba" + to_string(process_id) + ".jpg", imgrecive); */
    }
  }
  /* printf("smallImages size %ld", smallImages.size()); */
  /* cv::hconcat(smallImages, combined); */
  /* imwrite("salida.jpg", combined); */
  MPI_Finalize();
  return 0;
}
