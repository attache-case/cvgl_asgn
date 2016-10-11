#include <vector>
#include <sstream>
#include <iostream>
#include <opencv2/opencv.hpp>
 
//Global variables
int mode = 0;//control process in "main"
int init_state = 0;
cv::Point clicked_pt = cv::Point(-1, -1);
cv::Point2f srcPoint[4] = {cv::Point2f(-1, -1), cv::Point2f(-1, -1), cv::Point2f(-1, -1), cv::Point2f(-1, -1)};
cv::Point2f dstPoint[4] = {cv::Point2f(50, 100), cv::Point2f(87, 250), cv::Point2f(437, 250),cv::Point2f(550, 100)};

void init_calibration(cv::Mat frame);
void show_caribrated_image(cv::Mat frame, cv::Mat perspective_matrix);

void on_mouse_init(int event, int x, int y, int flags, void *) {
    if(event == CV_EVENT_LBUTTONDOWN){
        clicked_pt = cv::Point(x, y);
    }
}

/***************************
* main
***************************/
int main(){
    cv::VideoCapture cap(0);
    cv::namedWindow("initialization", 1);
    cv::setMouseCallback("initialization", on_mouse_init, 0);
    cv::Mat perspective_matrix;
    
    // esc を押すまで
    while (cv::waitKey(5) != 0x1b){
        cv::Mat frame;
        cap >> frame;
        // キャプチャできていなければ処理を飛ばす
        if (!frame.data){
            continue;
        }

        switch (mode) {
            case 0:
                init_calibration(frame);
                if(init_state == 4){
                    perspective_matrix = cv::getPerspectiveTransform(srcPoint, dstPoint);
                    init_state = 0;
                }
                break;
            case 1:
                show_caribrated_image(frame, perspective_matrix);
        }

    }
        
    return 0;        
}

void init_calibration(cv::Mat frame) {
    cv::Mat copy_frame;
    frame.copyTo(copy_frame);
    cv::rectangle(copy_frame, cv::Point(0, 0), cv::Point(640, 20), cv::Scalar::all(0), -1);
    switch (init_state) {
        case 0:
            cv::putText(copy_frame, "click the left-top of the Q key and press ENTER. (1/4)", cv::Point(20,15), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar::all(255), 1, CV_AA);
            break;
        case 1:
            cv::putText(copy_frame, "click the left-bottom of the Z key and press ENTER. (2/4)", cv::Point(20,15), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar::all(255), 1, CV_AA);
            break;
        case 2:
            cv::putText(copy_frame, "click the right-bottom of the M key and press ENTER. (3/4)", cv::Point(20,15), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar::all(255), 1, CV_AA);
            break;
        case 3:
            cv::putText(copy_frame, "click the right-top of the P key and press ENTER. (4/4)", cv::Point(20,15), cv::FONT_HERSHEY_TRIPLEX, 0.5, cv::Scalar::all(255), 1, CV_AA);
            break;
    }
    if( !(clicked_pt.x < 0 || clicked_pt.y < 0) ){
        cv::circle(copy_frame, cv::Point(clicked_pt.x, clicked_pt.y), 4, cv::Scalar::all(255), -1);
    }
    imshow("initialization", copy_frame);
    int k = cv::waitKey(33);
    switch (k) {
        case 10://ENTER
            if( !(clicked_pt.x < 0 || clicked_pt.y < 0) ) {
                srcPoint[init_state].x = clicked_pt.x;
                srcPoint[init_state].y = clicked_pt.y;
                clicked_pt = cv::Point(-1, -1);
                if(init_state < 3){
                    init_state++;
                } else {//init_state == 3
                    cv::destroyWindow("initialization");
                    init_state = 4;
                    mode = 1;//end initialization
                }
            } else {
                printf("point not selected!\n");
            }
            break;
        case 0x1b://esc
            exit(1);
            //break;
    }
}

void show_caribrated_image(cv::Mat frame, cv::Mat perspective_matrix){
    cv::Mat dst_img;
    cv::warpPerspective(frame, dst_img, perspective_matrix, frame.size(), cv::INTER_LINEAR);

    cv::line(frame, srcPoint[0], srcPoint[1], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, srcPoint[1], srcPoint[2], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, srcPoint[2], srcPoint[3], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, srcPoint[3], srcPoint[0], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, dstPoint[0], dstPoint[1], cv::Scalar(255,0,255), 2, CV_AA);
    cv::line(frame, dstPoint[1], dstPoint[2], cv::Scalar(255,0,255), 2, CV_AA);
    cv::line(frame, dstPoint[2], dstPoint[3], cv::Scalar(255,0,255), 2, CV_AA);
    cv::line(frame, dstPoint[3], dstPoint[0], cv::Scalar(255,0,255), 2, CV_AA);

    cv::namedWindow("src", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("dst", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("src", frame);
    cv::imshow("dst", dst_img);
    int k = cv::waitKey(33);
    switch (k) {
        case 0x1b://esc
            exit(1);
            //break;
    }
}