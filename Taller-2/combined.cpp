#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(){
    VideoCapture cap(0);
    if(!cap.isOpened()){
        return -1;
    }
    Mat frame, prevFrame, gray, prevGray, diff, thresh,avgFrame;
    cap >> prevFrame;
    cvtColor(prevFrame, prevGray, COLOR_BGR2GRAY);
    diff = Mat(prevGray.rows, prevGray.cols, CV_8U);
    thresh = Mat(prevGray.rows, prevGray.cols, CV_8U);
    avgFrame = prevGray.clone();
    float alpha = 0.5;
    while (true){
        cap >> frame;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        for (int i = 0; i < avgFrame.rows; i++) {
            for (int j = 0; j < avgFrame.cols; j++) {
                uchar currentPixel = gray.at<uchar>(i,j);
                uchar avgPixel = avgFrame.at<uchar>(i,j);
                float newValue = (1 - alpha) * currentPixel + alpha * avgPixel;
                avgFrame.at<uchar>(i,j) = static_cast<uchar>(newValue);
            }
        }
        for (int i = 0; i < gray.rows; i++) {
            for (int j = 0; j < gray.cols; j++) {
                diff.at<uchar>(i,j) = abs(gray.at<uchar>(i,j) - avgFrame.at<uchar>(i,j));
            }
        }
        for (int i = 0; i < diff.rows; i++) {
            for (int j = 0; j < diff.cols; j++) {
                thresh.at<uchar>(i,j) = (diff.at<uchar>(i,j) > 20) ? 255 : 0;
            }
        }
        imshow("Frame Difference", thresh);
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
}