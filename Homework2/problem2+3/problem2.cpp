#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Eigen/Dense>
using namespace cv;
using namespace std;
using namespace Eigen;
int n;

inline int rd(double x) {
    return (int)(x + 0.5);
}

int main() {
    freopen("points.txt", "r", stdin);
    cin >> n;
    Quaterniond q(-0.5, 0.5, 0.5, -0.5);
    Vector3d t(2., 2., 2.);
    Matrix4d pOut = [&q, &t]() {
        Matrix4d ret = Matrix4d::Zero();
        Matrix3d rot = q.matrix();
        ret.block(0, 0, 3, 3) = rot.transpose().cast<double>();
        ret.block(0, 3, 3, 1) = -rot.transpose().cast<double>() * t;
        ret(3, 3) = 1.;
        return ret;
    }();
    Matrix<double, 3, 4> pIn;
    pIn << 400., 0., 190., 0., 0., 400., 160., 0., 0., 0., 1., 0.;
    Matrix<double, 3, 4> p = pIn * pOut;
    Matrix<double, 4, 3500> a;
    for (int i = 0; i < n; i++) {
        cin >> a(0, i) >> a(1, i) >> a(2, i);
        a(3, i) = 1;
    }
    Matrix<double, 3, Dynamic> res = p * a;
    Mat ans = Mat::zeros(800, 800, CV_8UC1);
    for (int i = 0; i < n; i++) {
        if (res(2, i) < 0) continue; // back of camera
        res.col(i) /= res(2, i);
        int x = rd(100 + 0.5 * res(0, i)), y = rd(200 + 0.5 * res(1, i));
        swap(x, y); // opencv has reversed coordinates
        if (x < 0 || x >= 800 || y < 0 || y >= 800) continue;
        ans.at<uint8_t>(x, y) = static_cast<uint8_t>(255);
    }
    imshow("result", ans);
    waitKey();
    return 0;
}
