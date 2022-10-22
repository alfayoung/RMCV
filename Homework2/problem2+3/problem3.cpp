#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Eigen/Dense>
using namespace cv;
using namespace std;
using namespace Eigen;
int n;
Scalar col[5005];

inline int rd(double x) {
    return (int)(x + 0.5);
}

Matrix4d interpolate(Quaterniond st, Quaterniond ed, double t) {
    double cosa = st.w() * ed.w() + st.x() * ed.x() + st.y() * ed.y() + st.z() * ed.z();
    if (cosa < 1e-5) { ed = Vector4d(-ed.w(), -ed.x(), -ed.y(), -ed.z()); cosa = -cosa; }
    double k0, k1;
    if (cosa > 1. - 1e-5) {
        k0 = 1 - t, k1 = t;
    } else {
        double sina = sqrt(1 - cosa * cosa), a = atan2(sina, cosa);
        k0 = sin((1 - t) * a) / sina;
        k1 = sin(t * a) / sina;
    }
    Quaterniond q = {
        st.w() * k0 + ed.w() * k1, st.x() * k0 + ed.x() * k1,
        st.y() * k0 + ed.y() * k1, st.z() * k0 + ed.z() * k1
    };
    Vector3d p(2., 2., 2.);
    Matrix4d ret = Matrix4d::Zero();
    Matrix3d rot = q.matrix();
    ret.block(0, 0, 3, 3) = rot.transpose().cast<double>();
    ret.block(0, 3, 3, 1) = -rot.transpose().cast<double>() * p;
    ret(3, 3) = 1.;
    return ret;
}

int main() {
    freopen("points.txt", "r", stdin);
    cin >> n;
    srand(time(0));
    double stw = rand() % 10000, stx = rand() % 10000, sty = rand() % 10000, stz = rand() % 10000;
    double len = sqrt(stw * stw + stx * stx + sty * sty + stz * stz);
    stw /= len; stx /= len; sty /= len; stz /= len;
    Quaterniond st(stw, stx, sty, stz), ed(-0.5, 0.5, 0.5, -0.5);
    Matrix<double, 3, 4> pIn;
    pIn << 400., 0., 190., 0., 0., 400., 160., 0., 0., 0., 1., 0.;
    Matrix<double, 4, 3500> a;
    for (int i = 0; i < n; i++) {
        cin >> a(0, i) >> a(1, i) >> a(2, i);
        a(3, i) = 1;
        col[i] = Scalar(rand() % 56 + 200, rand() % 56 + 200, rand() % 56 + 200);
    }
    VideoWriter writer;
    writer.open("RM.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), 50, Size(1200, 800), true);
    assert(writer.isOpened());
    for (double t = 0; t <= 1; t += 0.005) {
        Matrix<double, 3, 4> p = pIn * interpolate(st, ed, t);
        Matrix<double, 3, Dynamic> res = p * a;
        Mat ans = Mat::zeros(800, 1200, CV_8UC3);
        for (int i = 0; i < n; i++) {
            if (res(2, i) < 0) continue; // back of camera
            double z = res(2, i), x = 300 + 0.6 * res(0, i) / z, y = 200 + 0.6 * res(1, i) / z;
            circle(ans, Point(x, y), 1, col[i], -1);
        }
        imshow("result", ans);
        writer.write(ans);
        waitKey(50);
    }
    writer.release();
    waitKey(0);
    return 0;
}