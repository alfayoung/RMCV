#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
using namespace std;
using namespace cv;
const double pi = acos(-1);

void show(Mat img) {
    imshow("img", img);
    waitKey();        
}

double dist(Point2f A, Point2f B) {
    return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y)); 
}

int cnt, lastflag, curflag;
Mat solve(Mat img, Mat orig) {
    Mat hsv, mask, img2;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    Scalar lb(35, 43, 46), ub(99, 255, 255);
    inRange(hsv, lb, ub, mask);
    bitwise_and(img, img, img2, mask);
    cvtColor(img2, img, COLOR_BGR2GRAY);
    threshold(img, img2, 50, 255, THRESH_BINARY);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(img2, img, kernel, Point(-1, -1), 1);
    morphologyEx(img, img2, MORPH_OPEN, kernel, Point(-1, -1), 1);
    Mat edge;
    Canny(img2, edge, 50, 100);
    vector<vector<Point> > contours;
    findContours(edge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //show(edge);

    curflag = 0; int tmp = 0;
    sort(contours.begin(), contours.end(), [](vector<Point> A, vector<Point> B) { return A[0].x > B[0].x; });
    for (auto c : contours) {
        RotatedRect rect = minAreaRect(c); Point2f P[4]; rect.points(P);
        double h = rect.size.height, w = rect.size.width;
        int id = cnt;
        if (h * w / contourArea(c) > 2 && h * w > 700
            && (min(h, w) > 20.5 || fabs(arcLength(c, true) - 2 * (w + h)) < 3)) {
            Moments m = moments(c);
            circle(orig, Point(m.m10 / m.m00, m.m01 / m.m00), 2, Scalar(0, 0, 255), 5);
            if (tmp && !(curflag && !lastflag)) id--; tmp = 1;
            string txt = "Bullet #" + to_string(id);
            putText(orig, txt, Point(m.m10 / m.m00 + 5, m.m01 / m.m00 + 5), FONT_ITALIC, 1, Scalar(0, 0, 255), 1);
            for (int i = 0; i < 4; i++) line(orig, P[i], P[(i + 1) % 4], Scalar(0, 0, 255));
            continue;
        } else if (min(h, w) > 20 || max(h, w) < 45) continue;
        pair<double, double> vec;
        if (dist(P[0], P[1]) > dist(P[0], P[3])) {
            vec.first = P[1].x - P[0].x;
            vec.second = P[1].y - P[0].y;
        } else {
            vec.first = P[3].x - P[0].x;
            vec.second = P[3].y - P[0].y;
        }
        if (acos(fabs(vec.second) / sqrt(vec.first * vec.first + vec.second * vec.second)) < pi / 5) {    
            curflag = 1;
            if (!lastflag) id++;
        }
        if (tmp && !(curflag && !lastflag)) id--; tmp = 1;
        circle(orig, Point((P[0].x + P[2].x) / 2, (P[0].y + P[2].y) / 2), 2, Scalar(0, 0, 255), 5);
        string txt = "Bullet #" + to_string(id);
        putText(orig, txt, Point((P[0].x + P[2].x) / 2 + 5, (P[0].y + P[2].y) / 2 + 5),
                FONT_ITALIC, 1, Scalar(0, 0, 255), 1);
        for (int i = 0; i < 4; i++) line(orig, P[i], P[(i + 1) % 4], Scalar(0, 0, 255));
    }
    if (!lastflag && curflag) cnt++;
    lastflag = curflag;
    show(orig);
    return orig;
}

int main() {
    VideoCapture capture("IMG_8511.mp4"); Mat src, lastSrc, diff;
    capture.read(src);
    VideoWriter writer;
    writer.open("result.mp4", VideoWriter::fourcc('M', 'P', '4', 'V'), 50, Size(src.cols, src.rows), true);
    int flag = 1;
    do {
        if (flag) diff = src.clone();
        else diff = src - lastSrc;
        flag = 0;
        lastSrc = src.clone();
        writer << solve(diff, src);
        waitKey(50);
    } while (capture.read(src));
    writer.release();
    return 0;
}