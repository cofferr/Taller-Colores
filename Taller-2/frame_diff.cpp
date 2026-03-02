#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(){
    VideoCapture cap(0);
    if(!cap.isOpened()){
        return -1;
    }
    Mat frame, prevFrame, gray, prevGray, diff, thresh;
    cap >> prevFrame;
    cvtColor(prevFrame, prevGray, COLOR_BGR2GRAY);
    diff = Mat(prevGray.rows, prevGray.cols, CV_8U);
    thresh = Mat(prevGray.rows, prevGray.cols, CV_8U);
    while (true){
        cap >> frame;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        for (int i = 0; i < gray.rows; i++) {
            for (int j = 0; j < gray.cols; j++) {
                diff.at<uchar>(i,j) = abs(gray.at<uchar>(i,j) - prevGray.at<uchar>(i,j));
            }
        }
        for (int i = 0; i < diff.rows; i++) {
            for (int j = 0; j < diff.cols; j++) {
                thresh.at<uchar>(i,j) = (diff.at<uchar>(i,j) > 10) ? 255 : 0;
            }
        }
        imshow("Frame Difference", thresh);
        prevGray = gray.clone();
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
}