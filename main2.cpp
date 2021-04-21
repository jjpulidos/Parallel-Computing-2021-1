#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <fstream>
#include <sstream>
#include "input.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

Mat img_hsv,img, new_h, new_s, new_v, dst_h, dst_s, dst_v;
int total_threads;
int ksize = 4;
vector<pair<int, int>> delta;
int* response;
pthread_t* my_threads;

void myfunction (int i) {  // function:
    std::cout << ' ' << i;
}


vector<uchar> medianFilterWindow(int i, int j){

    vector<uchar> pixelH(ksize * ksize);
    vector<uchar> pixelS(ksize * ksize);
    vector<uchar> pixelV(ksize * ksize);
    for (int k = 0; k < ksize * ksize; ++k){
        pixelH[k] = new_h.at<uchar>(i+delta[k].first, j+ delta[k].second);
        pixelS[k] = new_s.at<uchar>(i+delta[k].first, j+ delta[k].second);
        pixelV[k] = new_v.at<uchar>(i+delta[k].first, j+ delta[k].second);
    }
    sort(pixelH.begin(), pixelH.end());
    sort(pixelS.begin(), pixelS.end());
    sort(pixelV.begin(), pixelV.end());
    vector<uchar> pixels = {pixelH[(ksize * ksize) / 2], pixelS[(ksize * ksize) / 2], pixelV[(ksize * ksize) / 2]};
    return pixels;

}

void* medianFilter(void* id)
{

    int thread_id = *(int*) id;
    int n = new_h.rows/ total_threads;
    int start = n * thread_id;
    int end = start + n;
    vector<uchar> pixels;
    for (int i = start; i < end; i++){
        for(int j =0; j<new_h.cols-ksize;j++){
            pixels = medianFilterWindow(i, j);
            dst_h.at<uchar>(i, j) = pixels[0];
            dst_s.at<uchar>(i, j) = pixels[1];
            dst_v.at<uchar>(i, j) = pixels[2];

        }
    }

}


int main (int argc, char *argv[]) {

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

    total_threads = 12;

    img = getImg(argv[2]);
    cvtColor(img, img_hsv, COLOR_BGR2HSV);
    vector<Mat> hsvChannels(3);
    split(img_hsv, hsvChannels);
    
    for(int i = -1; i<ksize-1; i++){
        for(int j = -1; j<ksize-1; j++){
            delta.push_back(make_pair(i, j));
        }
    }

    new_h = hsvChannels[0];
    new_s = hsvChannels[1];
    new_v = hsvChannels[2];
    dst_h = new_h.clone();
    dst_s = new_s.clone();
    dst_v = new_v.clone();
    //Thread variables
    int threadId[total_threads], i, *retval;
    pthread_t thread[total_threads];

    auto start = high_resolution_clock::now();
    for(i = 0; i < total_threads; i++){
        threadId[i] = i;
        pthread_create(&thread[i], NULL, medianFilter, &threadId[i]);
    }

    //Join Threads
    for(i = 0; i < total_threads; i++){
        pthread_join(thread[i], (void **)&retval);
    }

    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);
    cout << "Tiempo de respuesta: " << total_time.count() / 1000 << '\n';
    vector<Mat> channels = {dst_h, dst_s, dst_v};

    Mat merged, filtered;
    merge(channels, merged);
    cvtColor(merged, filtered, COLOR_HSV2BGR);
    putImg(filtered, argv[3]);
    return 0;
}





