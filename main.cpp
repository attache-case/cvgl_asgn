#include <vector>
#include <sstream>
#include <iostream>
#include <windows.h> //ONLY AVAILABLE ON WINDOWS SYSYEM
#include <opencv2/opencv.hpp>
 
//Global variables
int mode = 0;//control process in "main"
int init_state = 0;
cv::Point clicked_pt = cv::Point(-1, -1);
cv::Point2f srcPoint[4] = {cv::Point2f(-1, -1), cv::Point2f(-1, -1), cv::Point2f(-1, -1), cv::Point2f(-1, -1)};
cv::Point2f dstPoint[4] = {cv::Point2f(50, 100), cv::Point2f(87, 250), cv::Point2f(437, 250),cv::Point2f(550, 100)};
cv::Point key_area_outerPoints[12] = {cv::Point(50, 100), cv::Point(50, 150), cv::Point(62, 150), cv::Point(62, 200), cv::Point(87, 200), cv::Point(87, 250),
                                      cv::Point(437, 250), cv::Point(437, 200), cv::Point(512, 200), cv::Point(512, 150), cv::Point(550, 150), cv::Point(550, 100)};
cv::Mat key_area_mask(480, 640, CV_8UC1);
cv::Mat caribrated_whole(480, 640, CV_8UC3);
bool caribrated = false;
cv::Point key_center[26] = {cv::Point(75, 125), cv::Point(125, 125), cv::Point(175, 125), cv::Point(225, 125), cv::Point(275, 125), cv::Point(325, 125), cv::Point(375, 125), cv::Point(425, 125), cv::Point(475, 125), cv::Point(525, 125),
                            cv::Point(87, 175), cv::Point(137, 175), cv::Point(187, 175), cv::Point(237, 175), cv::Point(287, 175), cv::Point(337, 175), cv::Point(387, 175), cv::Point(437, 175), cv::Point(487, 175),
                            cv::Point(112, 225), cv::Point(162, 225), cv::Point(212, 225), cv::Point(262, 225), cv::Point(312, 225), cv::Point(362, 225), cv::Point(412, 225)};
int char2keypos[26] = {10, 23, 21, 12, 2, 13, 14, 15, 7, 16, 17, 18, 25, 24, 8, 9, 0, 3, 11, 4, 6, 22, 1, 20, 5, 19};
int keypos2char[26] = {16, 22, 4, 17, 19, 24, 20, 8, 14, 15, 0, 18, 3, 5, 6, 7, 9, 10, 11, 25, 23, 2, 21, 1, 13, 12};

void init_calibration(cv::Mat frame);
void show_caribrated_image(cv::Mat frame, cv::Mat perspective_matrix);

void init() {
    //make key_area_mask
    cv::fillConvexPoly(key_area_mask, key_area_outerPoints, 12, cv::Scalar(255), CV_AA);
}

void on_mouse_init(int event, int x, int y, int flags, void *) {
    if(event == CV_EVENT_LBUTTONDOWN){
        clicked_pt = cv::Point(x, y);
    }
}

/***************************
* main
***************************/
int main(){
    init();

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
                    //save base caribrated img
                    cv::warpPerspective(frame, caribrated_whole, perspective_matrix, frame.size(), cv::INTER_LINEAR);
                    init_state = 0;
                    caribrated = true;
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
    cv::Mat bg(480, 640, CV_8UC3, cv::Scalar::all(0));
    cv::Mat blended;

    cv::warpPerspective(frame, dst_img, perspective_matrix, frame.size(), cv::INTER_LINEAR);

    cv::line(frame, srcPoint[0], srcPoint[1], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, srcPoint[1], srcPoint[2], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, srcPoint[2], srcPoint[3], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, srcPoint[3], srcPoint[0], cv::Scalar(255,255,0), 2, CV_AA);
    cv::line(frame, dstPoint[0], dstPoint[1], cv::Scalar(255,0,255), 2, CV_AA);
    cv::line(frame, dstPoint[1], dstPoint[2], cv::Scalar(255,0,255), 2, CV_AA);
    cv::line(frame, dstPoint[2], dstPoint[3], cv::Scalar(255,0,255), 2, CV_AA);
    cv::line(frame, dstPoint[3], dstPoint[0], cv::Scalar(255,0,255), 2, CV_AA);

    for (int i = 0; i < 26; i++)
    {
        cv::line(dst_img, key_center[i] + cv::Point(-25, -25), key_center[i] + cv::Point(25, -25), cv::Scalar(0,255,255), 2, CV_AA);
        cv::line(dst_img, key_center[i] + cv::Point(25, -25), key_center[i] + cv::Point(25, 25), cv::Scalar(0,255,255), 2, CV_AA);
        cv::line(dst_img, key_center[i] + cv::Point(25, 25), key_center[i] + cv::Point(-25, 25), cv::Scalar(0,255,255), 2, CV_AA);
        cv::line(dst_img, key_center[i] + cv::Point(-25, 25), key_center[i] + cv::Point(-25, -25), cv::Scalar(0,255,255), 2, CV_AA);
    }

    for (char c = 'A'; c <= 'Z'; c++)
    {
        if(GetAsyncKeyState(c) || GetAsyncKeyState(c+-'A'+'a'))
        {
            cv::rectangle(dst_img, key_center[char2keypos[c-'A']] + cv::Point(-25,-25), key_center[char2keypos[c-'A']] + cv::Point(25,25), cv::Scalar:all(255), -1);
        }
    }

    //overlay realtime image to base image
    cv::addWeighted(dst_img, 0.3, caribrated_whole, 0.7, 0.0, blended);

    dst_img.copyTo(bg, key_area_mask);

    cv::namedWindow("src", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("mask", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("dst", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("blended", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("src", frame);
    cv::imshow("mask", key_area_mask);
    cv::imshow("dst", bg);
    cv::imshow("blended", blended);
    int k = cv::waitKey(33);
    switch (k) {
        case 0x1b://esc
            exit(1);
            //break;
    }
}