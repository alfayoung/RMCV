#include <bits/stdc++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
using namespace cv;
using namespace Eigen;
using namespace std;

int main() {
    vector<Point2f> corners;
    corners.push_back(Point2f(575.508, 282.175));
    corners.push_back(Point2f(573.93, 331.819));
    corners.push_back(Point2f(764.518, 337.652));
    corners.push_back(Point2f(765.729, 286.741));
    vector<Point3d> PW_BIG{ {-0.115, 0.0265, 0.},
                                {-0.115, -0.0265, 0.},
                                {0.115, -0.0265, 0.},
                                {0.115, 0.0265, 0.} };
    Mat pIn = (Mat_<double>(3, 3) << 1.4142055480888289e+03, 0., 6.5792667877101735e+02, 0.,
               1.4137212634286254e+03, 4.0455967287503142e+02, 0., 0., 1.);
    Mat dist = (Mat_<double>(1, 5) << -1.5713532786218060e-01, 3.3883933349759310e-01,
                7.0103028435554145e-04, -2.9035486504482461e-04,
                -4.2372708010388316e-01);
    Mat rvec, tvec, Rtmp;
    solvePnP(PW_BIG, corners, pIn, dist, rvec, tvec);
    Rodrigues(rvec, Rtmp);

    MatrixXd R(3, 3), t(3, 1), T(3, 4);
    cv2eigen(Rtmp, R); cv2eigen(tvec, t);
    for (int i = 0; i < 4; i++) T.block<3, 1>(0, i) = t;
    
    MatrixXd PW(4, 3);
    PW << -0.115, 0.0265, 0.,
          -0.115, -0.0265, 0.,
          0.115, -0.0265, 0.,
          0.115, 0.0265, 0.;

    Quaterniond q(-0.0816168, 0.994363, -0.0676645, -0.00122528);
    MatrixXd PC(3, 4), PGyro(3, 4);
    PC = (R * PW.transpose() + T);
    PGyro = q.matrix() * PC;
    for (int i = 0; i < 3; i++) {
        double sum = 0;
        for (int j = 0; j < 4; j++) sum += PGyro(i, j);
        cout << sum / 4 << endl;
    }
    return 0;
}