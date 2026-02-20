#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(){
    VideoCapture cap(0);
    if(!cap.isOpened()){
        return -1;
    }

    Mat frame, gray, avgFrame;
    float alpha = 0.5;

    cap >> frame;
    cvtColor(frame, avgFrame, COLOR_BGR2GRAY);

    while (true){
        cap >> frame;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        for (int i = 0; i < avgFrame.rows; i++) {
            for (int j = 0; j < avgFrame.cols; j++) {

                uchar currentPixel = gray.at<uchar>(i,j);
                uchar avgPixel = avgFrame.at<uchar>(i,j);

                float newValue = alpha * currentPixel + (1 - alpha) * avgPixel;

                avgFrame.at<uchar>(i,j) = static_cast<uchar>(newValue);
            }
        }

        imshow("Running Average", avgFrame);

        if (waitKey(1) == 'q') {
            break;
        }
    }
}