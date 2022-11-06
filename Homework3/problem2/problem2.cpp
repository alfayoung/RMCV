#include <bits/stdc++.h>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
const int maxn = 35;
const int ZN = 1, XN = 2;
int n = 30;
double a[35];

int main() {
    freopen("dollar.txt", "r", stdin);
    freopen("predict.txt", "w", stdout);
    for (int i = 1; i <= n; i++) cin >> a[i];
    MatrixXd X(XN, 1);
    MatrixXd A(XN, XN), P(XN, XN), R(XN, XN), K(XN, XN);
    MatrixXd C(ZN, XN);
    MatrixXd Q(ZN, ZN);

    X << a[1], 0;
    A << 1, 1, 0, 1;
    P << 1, 0, 0, 1;
    C << 1, 0;
    R << 1, 0, 0, 1;
    Q << 15;

    for (int i = 2; i <= n; i++) {
        // predict
        MatrixXd Xk(XN, 1); Xk = A * X;
        P = A * P * A.transpose() + R;

        // update
        K = P * C.transpose() * (C * P * C.transpose() + Q).inverse();
        MatrixXd Z(ZN, 1); Z << a[i];
        X = Xk + K * (Z - C * Xk);
        P = (Matrix<double, XN, XN>::Identity() - K * C) * P;
        
        cout << X(0) << endl;
    }
    for (int i = 1; i <= 10; i++) {
        MatrixXd Xk(XN, 1); Xk = A * X;
        X = Xk;
        cout << X(0) << endl;        
    }
    return 0;
}