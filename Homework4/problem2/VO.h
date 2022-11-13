//
// Created by comoer on 2021/10/12.
//

#ifndef RM_STEREO_VO_H
#define RM_STEREO_VO_H
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

#define MIN_ERROR 0.01

void find_match(const Mat &img1,const Mat &img2,vector<Point2f>& lpt,vector<Point2f>& rpt);
void find_Trans_E(const Mat &img1,const Mat &img2,const Mat& K,Mat& R,Mat& t);
void find_Trans_H(const Mat &img1,const Mat &img2,Mat& H);
void process_Stitch(const Mat& img1,const Mat& img2,const Mat &img3,const Mat& H1,const Mat& H2);
void find_PnP(const Mat& img1,const Mat& depth1,const Mat& img2,const Mat& depth2,const Mat& K,Mat &rvec,Mat &tvec);
void find_use_E(const Mat &img1,const Mat& depth1,const Mat& img2,const Mat& depth2,const Mat& K,Mat& R,Mat& t);
void process_Stitch_project(const Mat &img1,const Mat& depth1,const Mat& img2,const Mat& depth2,const Mat& K,const Mat& R,const Mat& tvec,char* mask);
#endif //RM_STEREO_VO_H
