//
// Created by comoer on 2021/10/12.
//
#include "VO.h"

void find_match(const Mat &img1,const Mat &img2,vector<Point2f>& lpt,vector<Point2f>& rpt)
{
    Mat gray1,gray2;
    // using gray image to compute
    if(img1.dims == 3)
    {
        cvtColor(img1,gray1,COLOR_BGR2GRAY);
    }
    else{
        gray1 = img1;
    }
    if(img2.dims == 3)
    {
        cvtColor(img2,gray1,COLOR_BGR2GRAY);
    }
    else{
        gray2 = img2;
    }
    //create orb detector
    Ptr<ORB> orb = ORB::create();
    // create the container of Key points
    vector<KeyPoint> feature_points1,feature_points2;
    // do Orient_FAST detect Keypoint
    orb->detect(gray1,feature_points1);
    orb->detect(gray2,feature_points2);
    // compute the descriptors
    Mat descriptor1,descriptor2;
    orb->compute(gray1,feature_points1,descriptor1);
    orb->compute(gray2,feature_points2,descriptor2);
    //do matching
    //create Matcher
    BFMatcher matcher(NORM_HAMMING); //O(N^2)
    vector<DMatch> pairs;
    matcher.match(descriptor1,descriptor2,pairs);
    //You can also filter the match to generate
    double min_dist = 100000;
    // compute the minimum of the distance
    for(const DMatch&m:pairs)
    {
        if(m.distance < min_dist) min_dist = m.distance;
    }
    // filter
    for(const DMatch&m:pairs)
    {
        if(m.distance < max(min_dist*2,30.))
        {
            lpt.push_back(feature_points1[m.queryIdx].pt);
            rpt.push_back(feature_points2[m.trainIdx].pt);
        }
    }
}

void find_Trans_E(const Mat &img1,const Mat &img2,const Mat& K,Mat& R,Mat& t)
{
    vector<Point2f> left_pts;
    vector<Point2f> right_pts;
    find_match(img1,img2,left_pts,right_pts);

    // find the Essential Matrix using RANSAC
    Mat E = findEssentialMat(left_pts,right_pts,K,RANSAC);
    recoverPose(E,left_pts,right_pts,K,R,t);
}

void find_Trans_H(const Mat &img1,const Mat &img2,Mat& H)
{

    vector<Point2f> lpt,rpt;
    find_match(img1,img2,lpt,rpt);
    H = findHomography(lpt,rpt,RANSAC);

}
void process_Stitch(const Mat& img1,const Mat& img2,const Mat &img3,const Mat& H1,const Mat& H2)
{
    namedWindow("show",WINDOW_NORMAL);
    resizeWindow("show",800,600);
    Mat canvas;
    /*process img1 and img2 , i.e., middle and right*/
    /* adjust the H*/
    Mat H = H1.clone();
    for(int i = 0;i<3;++i)
    {
        H.at<double>(0,i) += H.at<double>(2,i)*double(img1.cols);
        H.at<double>(1,i) += H.at<double>(2,i)*double(img1.rows);
    }
    warpPerspective(img2,canvas,H,Size(3*img2.cols,3*img2.rows));
    /*process img1 and img3 , i.e., middle and left*/
    H = H2.clone();
    for(int i = 0;i<3;++i)
    {
        H.at<double>(0,i) += H.at<double>(2,i)*double(img1.cols);
        H.at<double>(1,i) += H.at<double>(2,i)*double(img1.rows);
    }
    Mat canvas2;
    warpPerspective(img3,canvas2,H,Size(3*img2.cols,3*img2.rows));
    add(canvas,canvas2,canvas);
    /*move the origin image to center*/
    img1.convertTo(canvas(Rect(img1.cols,img1.rows,img1.cols,img1.rows)),CV_8UC3);
    imshow("show",canvas);
    imwrite("../stitch.jpg",canvas);
    waitKey();
    destroyWindow("show");
}
void find_PnP(const Mat& img1,const Mat& depth1,const Mat& img2,const Mat& depth2,const Mat& K,Mat &rvec,Mat &tvec)
{
    vector<Point2f> lpt,rpt;
    find_match(img1,img2,lpt,rpt);
    vector<Point2f> cp2;
    undistortPoints(rpt,cp2,K,noArray());
    vector<Point3f> cp2_;
    /*reproject to 3D*/
    for(int i=0;i<rpt.size();++i)
    {
        float d = depth2.at<float>(int(rpt[i].y),int(rpt[i].x));
        cp2[i]*=d;
        cp2_.emplace_back(cp2[i].x,cp2[i].y,d);
    }

    solvePnPRansac(cp2_,lpt,K,noArray(),rvec,tvec);/*from img2 to img1*/
    vector<Point2f> check;
    projectPoints(cp2_,rvec,tvec,K,noArray(),check);
    double error = 0;
    for(int i = 0;i<check.size();++i)
    {
        error += norm((check[i]-lpt[i]));
    }
    error/=check.size();
    printf("pnp error is %.3f\n",error);
}

void find_use_E(const Mat &img1,const Mat& depth1,const Mat& img2,const Mat& depth2,const Mat& K,Mat& R,Mat& t)
{
    vector<Point2f> left_pts;
    vector<Point2f> right_pts;
    find_match(img2,img1,left_pts,right_pts);

    // find the Essential Matrix using RANSAC
    Mat E = findEssentialMat(left_pts,right_pts,K,RANSAC);
    recoverPose(E,left_pts,right_pts,K,R,t);

    /*using alpha formula*/
    double alpha = 0;
    vector<Point2f> crpt,clpt;
    /* to norm camera coordination*/
    undistortPoints(left_pts,clpt,K,noArray());
    undistortPoints(right_pts,crpt,K,noArray());
    int N = 0;
    for(int i=0;i<right_pts.size();++i)
    {
        Mat x1 = Mat(1,3,CV_64F);
        x1.at<double>(0,0) = clpt[i].x;
        x1.at<double>(0,1) = clpt[i].y;
        x1.at<double>(0,2) = 1;
        Mat x2 = Mat(3,1,CV_64F);
        x2.at<double>(0,0) = crpt[i].x;
        x2.at<double>(1,0) = crpt[i].y;
        x2.at<double>(2,0) = 1;
        Mat error = x1*E*x2;
        if(abs(error.at<double>(0,0))<MIN_ERROR) {
            Mat result = ((depth1.at<float>(right_pts[i].y,right_pts[i].x)*x2) -  R*(depth2.at<float>(left_pts[i].y,left_pts[i].x)*x1).t()).t()*t;
            alpha += result.at<double>(0,0);
            ++N;
        }
    }
    alpha = alpha/N/t.dot(t);
    t = alpha*t;

    vector<Point2f> cp2;
    undistortPoints(left_pts,cp2,K,noArray());
    vector<Point3f> cp2_;
    /*reproject to 3D*/
    for(int i=0;i<left_pts.size();++i)
    {
        float d = depth2.at<float>(left_pts[i].y,left_pts[i].x);
        cp2[i]*=d;
        cp2_.emplace_back(cp2[i].x,cp2[i].y,d);
    }

    Mat rvec;
    Rodrigues(R,rvec);
    vector<Point2f> check;
    projectPoints(cp2_,rvec,t,K,noArray(),check);
    double error = 0;
    for(int i = 0;i<check.size();++i)
    {
        error += norm((check[i]-right_pts[i]));
    }
    error /= check.size();
    printf("E error is %.3f\n",error);

}
void process_Stitch_project(const Mat &img1,const Mat& depth1,const Mat& img2,const Mat& depth2,const Mat& K,const Mat& R,const Mat& tvec,char* mask)
{
    char title[50];
    sprintf(title,"project_%s",mask);
    namedWindow(title,WINDOW_NORMAL);
    resizeWindow(title,800,600);
    /* from img2 to img1*/
    Mat rvec;
    Rodrigues(R,rvec);

    vector<Point2f> ip2;
    vector<Point2f> cp2_norm;/*x0,y0 on normal camera coordination*/
    vector<Point3f> cp2;

    for(int i = 0;i<img2.rows;++i)
        for(int j = 0;j<img2.cols;++j)
            ip2.emplace_back(j,i);

    undistortPoints(ip2,cp2_norm,K,noArray());
    for(int i = 0;i<img2.rows;++i)
        for(int j = 0;j<img2.cols;++j)
        {
            float d = depth2.at<float>(i,j);
            cp2.emplace_back(cp2_norm[i*img2.cols+j].x*d,cp2_norm[i*img2.cols+j].y*d,d);
        }
    vector<Point2f> project_ps2;
    projectPoints(cp2,rvec,tvec,K,noArray(),project_ps2);
    Mat canvas = Mat::zeros(3*img1.rows,3*img1.cols,CV_8UC3);
    img1.convertTo(canvas(Rect(img1.cols,img1.rows,img1.cols,img1.rows)),CV_8UC3);

    for(int i = 0;i<img2.rows;++i)
    {
        for(int j = 0;j<img2.cols;++j)
        {
            canvas.at<Vec3b>(cvRound(project_ps2[i*img2.cols+j].y)+img1.rows,cvRound(project_ps2[i*img2.cols+j].x)+img1.cols) = img2.at<Vec3b>(i,j)*2;
        }

    }

    imshow(title,canvas);
    waitKey();

    sprintf(title,"./project_%s.jpg",mask);
    imwrite(title,canvas);

}