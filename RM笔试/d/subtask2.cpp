#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
using namespace std;
using namespace cv;

void show(Mat img) {
    namedWindow("img");
    imshow("img", img);
    //waitKey();        
}

// get the distance between centroids of two points
double dist(int x, int y, vector<Point2f> ans) {
    double cx1 = 0, cy1 = 0;
    for (int j = 0; j < 4; j++) {
        cx1 += ans[(x - 1) * 4 + j].x;
        cy1 += ans[(x - 1) * 4 + j].y;
    }
    double cx2 = 0, cy2 = 0;
    for (int j = 0; j < 4; j++) {
        cx2 += ans[(y - 1) * 4 + j].x;
        cy2 += ans[(y - 1) * 4 + j].y;
    }
    cx1 /= 4; cy1 /= 4; cx2 /= 4; cy2 /= 4;
    return sqrt((cx1 - cx2) * (cx1 - cx2) + (cy1 - cy2) * (cy1 - cy2));
}

double interpolate(double x) {
    if (0 <= x && x <= 350) return x;
    else if (x <= 500) return max(0.9 * x, 350.);
    else if (x <= 700) return max(0.85 * x, 450.);
    else return max(0.8 * x, 595.);
}

int perm[105], best[105];
void solve(Mat orig) {
    Mat img, hsv, edge;
    GaussianBlur(orig, img, Size(7, 7), 15, 0, 4);
    threshold(img, img, 100, 255, THRESH_BINARY);
    cvtColor(img, hsv, COLOR_BGR2HSV);
    
    Mat maskWhite, maskSkyBlue, maskPurple; // White for lights; Skyblue for front lights
    Scalar lb = Scalar{0, 0, 221}, ub = Scalar{180, 30, 255};
    inRange(hsv, lb, ub, maskWhite);
    lb = Scalar(70, 43, 46); ub = Scalar(90, 255, 255);
    inRange(hsv, lb, ub, maskSkyBlue);
    Mat mask = maskWhite + maskSkyBlue;
    Mat board;
    bitwise_and(img, img, board, mask);
    //show(board);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    //morphologyEx(board, board, MORPH_OPEN, kernel, Point(-1, -1), 2);
    morphologyEx(board, board, MORPH_CLOSE, kernel, Point(-1, -1), 3);
    cvtColor(board, board, COLOR_BGR2GRAY);
    threshold(board, board, 127, 255, THRESH_BINARY);
    //show(board);

    Canny(board, edge, 100, 200);
    //show(edge);

    vector<vector<Point> > contours;
    findContours(edge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    double mxArea = 0;
    for (auto c : contours) {
        double area = contourArea(c);
        if (area > 1500 || area < 75) continue;
        vector<Point> hull;
        convexHull(c, hull);
        if (contourArea(hull) > area * 1.2) continue;
        RotatedRect res = minAreaRect(c);
        Point2f P[4]; res.points(P);
        double mxx = max(abs(P[0].x - P[2].x), abs(P[1].x - P[3].x));
        double mxy = max(abs(P[0].y - P[2].y), abs(P[1].y - P[3].y));
        if (mxx > 2 * mxy) continue;
        mxArea = max(mxArea, area);
    }   

    vector<Point2f> ans;
    vector<double> Area;
    for (auto c : contours) {
        double area = contourArea(c);
        if (area > 1500 || area < mxArea * 0.3) continue;
        vector<Point> hull;
        convexHull(c, hull);
        cout << contourArea(hull) << " " << area << endl;
        if (contourArea(hull) > area * 1.2) continue; // filter out front board which is concave
        RotatedRect res = minAreaRect(c);
        Point2f P[4]; res.points(P);
        double mxx = max(abs(P[0].x - P[2].x), abs(P[1].x - P[3].x));
        double mxy = max(abs(P[0].y - P[2].y), abs(P[1].y - P[3].y));
        if (mxx > 2 * mxy) continue;
        for (int i = 0; i < 4; i++) ans.push_back(P[i]);
        Area.push_back(area);
    }
    int n = ans.size() / 4;
    cout << n << endl;
    for (int i = 1; i <= n; i++) perm[i] = i;

    // match the lights according to distance
    double Best = 1e18;
    do {
        double res = 0;
        for (int i = 1; i <= n; i += 2) {
            if (i == n) break;
            double dis = dist(perm[i], perm[i + 1], ans);
            // thres is the tolerance distance between two lights which together form a board
            double thres = 100 * sqrt(interpolate(max(Area[perm[i] - 1], Area[perm[i + 1] - 1])) / 250.);
            if (dis < thres) res += dis;
            else res += 2 * thres;
        }
        if (res < Best) {
            for (int i = 1; i <= n; i++) best[i] = perm[i];
            Best = res;
        }
    } while (next_permutation(perm + 1, perm + n + 1));
    
    for (int i = 1, match = 0; i <= n;) {
        vector<Point2f> tmp;
        double thres = (i == n) ? 1e9 : 100 * sqrt(interpolate(max(Area[best[i] - 1], Area[best[i + 1] - 1])) / 250.);
        if (i == n || match >= 2 || dist(best[i], best[i + 1], ans) >= thres) {
            for (int j = 0; j < 4; j++) {
                tmp.push_back(ans[(best[i] - 1) * 4 + j]);
            }
            i++;
        } else {
            cout << dist(best[i], best[i + 1], ans) << " and  " << thres << endl;
            for (int j = 0; j < 4; j++) {
                tmp.push_back(ans[(best[i] - 1) * 4 + j]);
                tmp.push_back(ans[(best[i + 1] - 1) * 4 + j]);
            }   
            i += 2; match++;
        }
        RotatedRect res = minAreaRect(tmp);
        Point2f P[4]; res.points(P);
        for (int j = 0; j < 4; j++) {
            line(orig, P[j], P[(j + 1) % 4], Scalar(0, 255, 0), 2);
        }        
    }

    show(orig);
}

int main() {
    cv::VideoCapture capture("d.mp4"); Mat src;
    while (capture.read(src)) {
        solve(src);
        cv::waitKey(50);
    }
    return 0;
}
