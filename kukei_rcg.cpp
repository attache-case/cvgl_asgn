/***************************
* 矩形検出
* @author : tetro
***************************/
#include <vector>
#include <sstream>
#include <iostream>
#include <opencv2/opencv.hpp>
 
/***************************
* main
***************************/
int main(){
    cv::VideoCapture cap(0);
    // esc を押すまで
    while (cv::waitKey(5) != 0x1b){
        cv::Mat frame;
        cap >> frame;
        // キャプチャできていなければ処理を飛ばす
        if (!frame.data){
            continue;
        }
 
        // 2値化
        cv::Mat grayImage, binImage, im_blue, im_green, im_red;
        std::vector<cv::Mat> planes;
        cv::cvtColor(frame, grayImage, CV_BGR2GRAY);
        cv::split(frame, planes);
        im_blue = planes[0];
        im_green = planes[1];
        im_red = planes[2];
        cv::threshold(grayImage, binImage, 128.0, 255.0, CV_THRESH_OTSU);
        cv::threshold(planes[0], im_blue, 128.0, 255.0, CV_THRESH_OTSU);
        cv::threshold(planes[1], im_green, 128.0, 255.0, CV_THRESH_OTSU);
        cv::threshold(planes[2], im_red, 128.0, 255.0, CV_THRESH_OTSU);
        //binImage = binImage + im_blue + im_green + im_red;
        cv::imshow("bin", binImage);
        cv::imshow("bin2", binImage + im_blue + im_green + im_red);
 
        // 輪郭抽出
        std::vector< std::vector< cv::Point > > contours;
        cv::findContours(binImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        // 検出された輪郭線の描画
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            cv::polylines(frame, *contour, true, cv::Scalar(0, 255, 0), 2);
        }
 
        // 輪郭が四角形かの判定
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            // 輪郭を直線近似
            std::vector< cv::Point > approx;
            cv::approxPolyDP(cv::Mat(*contour), approx, 50.0, true);
            // 近似が4線かつ面積が一定以上なら四角形
            double area = cv::contourArea(approx);
            if (approx.size() == 4 && area > 1000.0){
                cv::polylines(frame, approx, true, cv::Scalar(255, 0, 0), 2);
                std::stringstream sst;
                sst << "area : " << area;
                cv::putText(frame, sst.str(), approx[0], CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 128, 0));
            }
        }
        cv::imshow("frame", frame);
    }
}
