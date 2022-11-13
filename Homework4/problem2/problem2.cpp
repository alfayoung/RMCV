#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "VO.h"
using namespace cv;
using namespace std;

int main() {
    Mat imgL = imread("./stereo-data/0_orig.jpg");
    Mat depthL = imread("./stereo-data/0_dpt.tiff", cv::IMREAD_ANYDEPTH);
    Mat imgR = imread("./stereo-data/1_orig.jpg");
    Mat depthR = imread("./stereo-data/1_dpt.tiff", cv::IMREAD_ANYDEPTH);
    Mat K, R, rvec, tvec;
    FileStorage params("camera.yaml", FileStorage::READ);
    K = params["K"].mat();
    find_PnP(imgL, depthL, imgR, depthR, K, rvec, tvec);
    Rodrigues(rvec, R);
    process_Stitch_project(imgL, depthL, imgR, depthR, K, R, tvec, "res");
    return 0;
}