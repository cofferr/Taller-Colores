#include "gmm_segmenter.h"

using namespace cv;
using namespace std;

GMMSegmenter::GMMSegmenter(const Config& cfg)
    : cfg_(cfg)
{
    pMOG2_ = createBackgroundSubtractorMOG2(
        cfg_.history,
        cfg_.var_threshold,
        cfg_.detect_shadows
    );

    kernel_open_  = getStructuringElement(MORPH_RECT, 
                     Size(cfg_.morph_open_k, cfg_.morph_open_k));

    kernel_close_ = getStructuringElement(MORPH_RECT, 
                     Size(cfg_.morph_close_k, cfg_.morph_close_k));
}


Mat GMMSegmenter::apply(const Mat& frame)
{
    Mat fg_mask;
    Mat clean_mask;

    pMOG2_->apply(frame, fg_mask, -1);

    threshold(fg_mask, clean_mask, 200, 255, THRESH_BINARY);

    morphologyEx(clean_mask, clean_mask, MORPH_OPEN, kernel_open_);

    morphologyEx(clean_mask, clean_mask, MORPH_CLOSE, kernel_close_);

    return clean_mask;
}


vector<Rect> GMMSegmenter::getRegions(const Mat& mask) const
{
    vector<vector<Point>> contours;
    vector<Rect> regions;

    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for(const auto& contour : contours)
    {
        double area = contourArea(contour);

        if(area >= cfg_.min_area)
        {
            regions.push_back(boundingRect(contour));
        }
    }

    return regions;
}


Mat GMMSegmenter::getBackground() const
{
    Mat background;
    pMOG2_->getBackgroundImage(background);
    return background;
}