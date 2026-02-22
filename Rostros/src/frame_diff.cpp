#include "frame_diff.h"

using namespace cv;
using namespace std;

FrameDifferencer::FrameDifferencer(int threshold_val, int min_contour_area, double learning_rate)
{
    threshold_val_ = threshold_val;
    min_area_ = min_contour_area;
    learning_rate_ = learning_rate;

    kernel_ = getStructuringElement(MORPH_RECT, Size(3,3));
}


void FrameDifferencer::setBackground(const Mat& frame)
{
    Mat gray;

    if(frame.channels() == 3)
        cvtColor(frame, gray, COLOR_BGR2GRAY);
    else
        gray = frame.clone();

    GaussianBlur(gray, background_, Size(7,7), 0);

    background_.convertTo(background_float_, CV_32F);
}

Mat FrameDifferencer::process(const Mat& frame)
{
    Mat gray, gray_blur, diff, mask;

    if(frame.channels() == 3)
        cvtColor(frame, gray, COLOR_BGR2GRAY);
    else
        gray = frame.clone();

    GaussianBlur(gray, gray_blur, Size(7,7), 0);

    absdiff(background_, gray_blur, diff);

    threshold(diff, mask, threshold_val_, 255, THRESH_BINARY);

    morphologyEx(mask, mask, MORPH_OPEN, kernel_);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel_);

    if(learning_rate_ > 0.0)
        updateBackground(gray);

    return mask;
}


vector<Rect> FrameDifferencer::getRegions(const Mat& mask)
{
    vector<vector<Point>> contours;
    vector<Rect> regions;

    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for(const auto& contour : contours)
    {
        double area = contourArea(contour);

        if(area >= min_area_)
        {
            Rect box = boundingRect(contour);
            regions.push_back(box);
        }
    }

    return regions;
}


void FrameDifferencer::updateBackground(const Mat& frame)
{
    Mat gray;

    if(frame.channels() == 3)
        cvtColor(frame, gray, COLOR_BGR2GRAY);
    else
        gray = frame;

    Mat gray_float;
    gray.convertTo(gray_float, CV_32F);

    accumulateWeighted(gray_float, background_float_, learning_rate_);

    background_float_.convertTo(background_, CV_8U);
}