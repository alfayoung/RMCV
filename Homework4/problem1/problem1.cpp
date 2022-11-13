#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
using namespace cv;
using namespace std;

int main() {
    namedWindow("show", WINDOW_NORMAL);
    resizeWindow("show", 800, 600);
    Mat img1 = imread("9407.png");
    Mat img2 = imread("9408.png");
    Mat gray1, gray2;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    cvtColor(img2, gray2, COLOR_BGR2GRAY);

    Ptr<ORB> orb = ORB::create();

    vector<KeyPoint> kp1, kp2;
    orb->detect(gray1, kp1);
    orb->detect(gray2, kp2);

    Mat desc1, desc2;
    orb->compute(gray1, kp1, desc1);
    orb->compute(gray2, kp2, desc2);

    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> pairs;
    matcher.match(desc1, desc2, pairs);

    vector<DMatch> goodPairs;
    double mn = 1e6;
    for (DMatch m : pairs) mn = min(mn, (double)m.distance);
    for (DMatch m : pairs) if (m.distance < max(mn * 2, 30.)) goodPairs.push_back(m);

    Mat canvas;
    drawMatches(img1, kp1, img2, kp2, goodPairs, canvas);
    imshow("show", canvas);
    waitKey(0);
    return 0;
}