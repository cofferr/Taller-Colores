#include <opencv2/opencv.hpp>
#include <iostream>
#include "gmm_segmenter.h"

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0);

    if(!cap.isOpened())
    {
        cout << "Error abriendo camara" << endl;
        return -1;
    }

    GMMSegmenter::Config cfg;
    cfg.history = 500;
    cfg.var_threshold = 16.0;
    cfg.detect_shadows = true;
    cfg.min_area = 1500;

    GMMSegmenter gmm(cfg);

    Mat frame;

    while(true)
    {
        cap >> frame;
        if(frame.empty())
            break;

        Mat mask = gmm.apply(frame);

        vector<Rect> regions = gmm.getRegions(mask);

        for(const auto& box : regions)
        {
            rectangle(frame, box, Scalar(0,255,0), 2);
        }

        Mat background = gmm.getBackground();

        imshow("Frame", frame);
        imshow("FG Mask", mask);

        if(!background.empty())
            imshow("Background (GMM)", background);

        if(waitKey(30) == 27)
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}