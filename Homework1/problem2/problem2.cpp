#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
using namespace std;
using namespace cv;

int main() {
    Mat img = imread("apple.png"), hsv, img2, orig = img.clone();
    cvtColor(img, hsv, COLOR_BGR2HSV);
    Mat mask1, mask2, mask3;
    Scalar lb = {0, 170, 100}, ub = {10, 255, 255};
    inRange(hsv, lb, ub, mask1);
    lb = {156, 170, 100}, ub = {180, 255, 255};
    inRange(hsv, lb, ub, mask2);
    lb = {11, 170, 100}, ub = {25, 255, 255};
    inRange(hsv, lb, ub, mask3);
    bitwise_and(img, img, img2, mask1 + mask2 + mask3);
    threshold(img2, img, 100, 255, THRESH_BINARY);
    cvtColor(img, img2, COLOR_BGR2GRAY);
    threshold(img2, img, 100, 255, THRESH_TOZERO_INV);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(img, img2, MORPH_OPEN, kernel, Point(-1, -1), 3);
    morphologyEx(img2, img, MORPH_CLOSE, kernel, Point(-1, -1), 5);
    Mat edge;
    Canny(img, edge, 50, 100);
    vector<vector<Point> > contours;
    findContours(edge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (auto c : contours) {
        if (contourArea(c) > 1000) {
            rectangle(orig, boundingRect(c), Scalar(0, 255, 0), 2, 8, 0);
        }
    }
    imwrite("appleDetected.jpg", orig);
    imshow("img", orig);
    waitKey();
    return 0;
}