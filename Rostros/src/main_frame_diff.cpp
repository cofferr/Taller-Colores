#include <opencv2/opencv.hpp>
#include "frame_diff.h"
#include <iostream>

using namespace cv;
using namespace std;

int main(){
    VideoCapture cap(0);

    if(!cap.isOpened())
    {
        cout << "Error abriendo video" << endl;
        return -1;
    }
    Mat frame;
    cap >> frame;

    if(frame.empty())
    {
        cout << "Frame vacío" << endl;
        return -1;
    }
    FrameDifferencer differencer(
        50,
        500,
        0.5
    );

    differencer.setBackground(frame);

    while(true)
    {
        cap >> frame;

        if(frame.empty())
            break;
        Mat mask = differencer.process(frame);

        vector<Rect> regions = differencer.getRegions(mask);

        for(const auto& box : regions)
        {
            rectangle(frame, box, Scalar(0,255,0), 2);
        }

        imshow("Frame", frame);
        imshow("Mask", mask);

        int key = waitKey(30);
        if(key == 27)
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}