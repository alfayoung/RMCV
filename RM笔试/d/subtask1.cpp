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
	waitKey();
}

double dis(Point2f A, Point2f B) {
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

int main() {
	Mat orig = imread("d.jpg"), edge, hsv, img;
	GaussianBlur(orig, img, Size(7, 7), 15, 0, 4);
	threshold(img, img, 100, 255, THRESH_BINARY);
	show(img);
	cvtColor(img, hsv, COLOR_BGR2HSV);

	Mat mask;
	Scalar lb = Scalar{0, 0, 221}, ub = Scalar{180, 30, 255};
	inRange(hsv, lb, ub, mask);
	Mat board;
	bitwise_and(img, img, board, mask);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(board, board, MORPH_OPEN, kernel, Point(-1, -1), 2);
	morphologyEx(board, board, MORPH_CLOSE, kernel, Point(-1, -1), 3);

	Canny(board, edge, 200, 400);

	vector<vector<Point> > contours;
	findContours(edge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Point2f> ans;
	for (auto c : contours) {
		double area = contourArea(c);
		if (area < 1000) continue;
		RotatedRect res = minAreaRect(c);
		Point2f P[4]; res.points(P);
		for (int i = 0; i < 4; i++) ans.push_back(P[i]);
		// double w = dis(P[0], P[1]), h = dis(P[1], P[2]);
	}
	RotatedRect res = minAreaRect(ans);
	Point2f P[4]; res.points(P);
	for (int i = 0; i < 4; i++) {
		line(orig, P[i], P[(i + 1) % 4], Scalar(0, 255, 0), 2);
	}
	show(orig);
	return 0;
}
