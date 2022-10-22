#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgcodecs.hpp>
using namespace std;
using namespace cv;

int main() {
    Mat img = imread("1634287874477.png"), orig = img.clone(), hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    Scalar lb = {100, 70, 70}, ub = {124, 255, 255};
    Mat mask, img2;
    inRange(hsv, lb, ub, mask);
    bitwise_and(img, img, img2, mask);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(img2, img, MORPH_CLOSE, kernel, Point(-1, -1), 5);
    Canny(img, img2, 50, 100);
    vector<vector<Point> > contours, poly;
    findContours(img2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    poly.resize(1);
    for (int i = 0; i < contours.size(); i++) {
        if (contourArea(contours[i]) < 10000) continue;
        approxPolyDP(contours[i], poly[0], 10, true);
    }
    drawContours(orig, poly, 0, Scalar(0, 255, 0));
    // Point2f src[] = {Point2f(741, 411), Point2f(752, 489), Point2f(958, 415), Point2f(966, 487)};
    Point2f src[] = {Point2f(poly[0][0]), Point2f(poly[0][1]), Point2f(poly[0][3]), Point2f(poly[0][2])};
    int w = 440, h = 140;
    Point2f dst[] = {Point2f(0, 0), Point2f(0, h - 1), Point2f(w - 1, 0), Point2f(w - 1, h - 1)};
    Mat trans = getPerspectiveTransform(src, dst), res;
    warpPerspective(orig, res, trans, Size(w, h));
    imshow("result", res);
    waitKey();
    return 0;
}