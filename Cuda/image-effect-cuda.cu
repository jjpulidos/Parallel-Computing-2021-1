#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "input.hpp"
#include <cuda.h>
#include <cuda_runtime.h>
#include <opencv2/opencv.hpp>
#include <typeinfo>

using namespace std;
using namespace cv;
using namespace chrono;

Mat img_hsv, img, new_h, new_s, new_v, dst_h, dst_s, dst_v;
int total_threads;
int ksize;
vector<pair<int, int>> delta;

__global__ void median_filter_thread(const uchar *inputImageKernel,
                                     uchar *outputImagekernel,
                                     const int imageWidth,
                                     const int imageHeight) {
  // Set row and colum for thread.
  int WINDOW_SIZE = 3;
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned char filterVector[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  if ((row == 0) || (col == 0) || (row == imageHeight - 1) ||
      (col == imageWidth - 1)) {
    outputImagekernel[row * imageWidth + col] = 0;
  } else {
    for (int x = 0; x < WINDOW_SIZE; x++) {
      for (int y = 0; y < WINDOW_SIZE; y++) {
        filterVector[x * WINDOW_SIZE + y] =
            inputImageKernel[(row + x - 1) * imageWidth + (col + y - 1)];
      }
    }
    for (int i = 0; i < 9; i++) {
      for (int j = i + 1; j < 9; j++) {
        if (filterVector[i] > filterVector[j]) {
          // Swap the variables.
          char tmp = filterVector[i];
          filterVector[i] = filterVector[j];
          filterVector[j] = tmp;
        }
      }
    }
    outputImagekernel[row * imageWidth + col] = filterVector[4];

    /* outputImagekernel[row * imageWidth + col] = */
    /*     inputImageKernel[row * imageWidth + col]; */
  }
}

int main(int argc, char *argv[]) {

  // Getting Arguments from execution command
  checkNumArgs(6, argc, argv[0]);

  // Parsing Arguments
  int blocksPerGrid = parsePosInt(argv[1]);
  int num_threads_x = parsePosInt(argv[4]);
  int num_threads_y = parsePosInt(argv[5]);
  img = getImg(argv[2]);

  cvtColor(img, img_hsv, COLOR_BGR2HSV);
  vector<Mat> hsvChannels(3);
  split(img_hsv, hsvChannels);

  Mat new_h = hsvChannels[0];
  Mat new_s = hsvChannels[1];
  Mat new_v = hsvChannels[2];

  cudaError_t err = cudaSuccess;

  // flatten the mat.
  uint totalElements = img.total(); // Note: image.total() == rows*cols.
  Mat flat_h =
      new_h.reshape(1, totalElements); // 1xN mat of 1 channel, O(1) operation
  Mat flat_s =
      new_s.reshape(1, totalElements); // 1xN mat of 1 channel, O(1) operation
  Mat flat_v =
      new_v.reshape(1, totalElements); // 1xN mat of 1 channel, O(1) operation

  if (!new_h.isContinuous()) {
    flat_h = flat_h.clone(); // O(N),
  }
  if (!new_s.isContinuous()) {
    flat_s = flat_s.clone(); // O(N),
  }
  if (!new_v.isContinuous()) {
    flat_v = flat_v.clone(); // O(N),
  }

  // flat.data is your array pointer
  auto *h_img_or_h = flat_h.data; // usually, its uchar*
  auto *h_img_or_s = flat_s.data; // usually, its uchar*
  auto *h_img_or_v = flat_v.data; // usually, its uchar*

  size_t size = img.rows * img.cols * sizeof(char);

  uchar *d_img_or_h = NULL;
  err = cudaMalloc((void **)&d_img_or_h, size);

  if (err != cudaSuccess) {
    cout << "Error separando espacio imagen normal en GPU "
         << cudaGetErrorString(err) << endl;
    return -1;
  }

  uchar *d_img_or_s = NULL;
  err = cudaMalloc((void **)&d_img_or_s, size);

  if (err != cudaSuccess) {
    cout << "Error separando espacio imagen normal en GPU "
         << cudaGetErrorString(err) << endl;
    return -1;
  }

  uchar *d_img_or_v = NULL;
  err = cudaMalloc((void **)&d_img_or_v, size);

  if (err != cudaSuccess) {
    cout << "Error separando espacio imagen normal en GPU "
         << cudaGetErrorString(err) << endl;
    return -1;
  }

  uchar *d_img_mf_h = NULL;

  err = cudaMalloc((void **)&d_img_mf_h, size);
  if (err != cudaSuccess) {
    cout << "Error separando espacio imagen normal en GPU 2"
         << cudaGetErrorString(err) << endl;
    return -1;
  }

  uchar *d_img_mf_s = NULL;

  err = cudaMalloc((void **)&d_img_mf_s, size);
  if (err != cudaSuccess) {
    cout << "Error separando espacio imagen normal en GPU 2"
         << cudaGetErrorString(err) << endl;
    return -1;
  }

  uchar *d_img_mf_v = NULL;

  err = cudaMalloc((void **)&d_img_mf_v, size);
  if (err != cudaSuccess) {
    cout << "Error separando espacio imagen normal en GPU 2"
         << cudaGetErrorString(err) << endl;
    return -1;
  }

  uchar *h_img_mf_h = (uchar *)malloc(size);
  uchar *h_img_mf_s = (uchar *)malloc(size);
  uchar *h_img_mf_v = (uchar *)malloc(size);

  // MemoryCopy
  // Imagen
  err = cudaMemcpy(d_img_or_h, h_img_or_h, size, cudaMemcpyHostToDevice);
  if (err != cudaSuccess) {
    cout << "Error copiando datos a GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }
  err = cudaMemcpy(d_img_or_s, h_img_or_s, size, cudaMemcpyHostToDevice);
  if (err != cudaSuccess) {
    cout << "Error copiando datos a GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }
  err = cudaMemcpy(d_img_or_v, h_img_or_v, size, cudaMemcpyHostToDevice);
  if (err != cudaSuccess) {
    cout << "Error copiando datos a GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }

  // Lanzar GPU

  const dim3 block(blocksPerGrid, blocksPerGrid);
  if (num_threads_x == 0 || num_threads_y == 0) {
    num_threads_x = (int)ceil((float)img.cols / (float)blocksPerGrid);
    num_threads_y = (int)ceil((float)img.rows / (float)blocksPerGrid);
  }
  const dim3 grid(num_threads_x, num_threads_y);
  /* cout << (int)ceil((float)img.cols / (float)blocksPerGrid) << endl; */
  /* cout << (int)ceil((float)img.rows / (float)blocksPerGrid) << endl; */

  // Start timing
  auto start = high_resolution_clock::now();

  median_filter_thread<<<grid, block>>>(d_img_or_h, d_img_mf_h, img.cols,
                                        img.rows);

  err = cudaGetLastError();
  if (err != cudaSuccess) {
    cout << "Fallo al lanzar Kernel de GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }
  median_filter_thread<<<grid, block>>>(d_img_or_s, d_img_mf_s, img.cols,
                                        img.rows);

  err = cudaGetLastError();
  if (err != cudaSuccess) {
    cout << "Fallo al lanzar Kernel de GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }
  median_filter_thread<<<grid, block>>>(d_img_or_v, d_img_mf_v, img.cols,
                                        img.rows);

  err = cudaGetLastError();
  if (err != cudaSuccess) {
    cout << "Fallo al lanzar Kernel de GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }
  cudaDeviceSynchronize();

  // End timing
  auto end = high_resolution_clock::now();

  err = cudaMemcpy(h_img_mf_h, d_img_mf_h, size, cudaMemcpyDeviceToHost);
  if (err != cudaSuccess) {
    cout << "Error copiando datos a GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }
  err = cudaMemcpy(h_img_mf_s, d_img_mf_s, size, cudaMemcpyDeviceToHost);
  if (err != cudaSuccess) {
    cout << "Error copiando datos a GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }

  err = cudaMemcpy(h_img_mf_v, d_img_mf_v, size, cudaMemcpyDeviceToHost);
  if (err != cudaSuccess) {
    cout << "Error copiando datos a GPU " << cudaGetErrorString(err) << endl;
    return -1;
  }

  duration<double, milli> total_time = (end - start);
  cout << "time    = " << total_time.count() / 1000 << '\n';

  Mat dst_h = Mat(img.rows, img.cols, CV_8UC1, h_img_mf_h);
  Mat dst_s = Mat(img.rows, img.cols, CV_8UC1, h_img_mf_s);
  Mat dst_v = Mat(img.rows, img.cols, CV_8UC1, h_img_mf_v);

  vector<uchar> vec(h_img_mf_h, h_img_mf_h + flat_h.total());

  // After applied MedianFilter for each channel, were merged due to build the
  // Image Again
  vector<Mat> channels = {dst_h, dst_s, dst_v};
  Mat merged, filtered;
  merge(channels, merged);

  // Changing HSV mode color to BGR again
  cvtColor(merged, filtered, COLOR_HSV2BGR);

  putImg(filtered, argv[3]);

  // Saving Image results
  cudaFree(d_img_or_h);
  cudaFree(d_img_or_s);
  cudaFree(d_img_or_v);
  cudaFree(d_img_mf_h);
  cudaFree(d_img_mf_s);
  cudaFree(d_img_mf_v);
  /* free(h_img_or_h); */
  /* free(h_img_or_s); */
  /* free(h_img_or_v); */
  free(h_img_mf_h);
  free(h_img_mf_s);
  free(h_img_mf_v);
}
