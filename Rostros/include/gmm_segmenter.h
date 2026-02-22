#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/bgsegm.hpp>
#include <vector>

class GMMSegmenter {
public:

    struct Config {
        int history;
        double var_threshold;
        bool detect_shadows;
        int min_area;
        int morph_open_k;
        int morph_close_k;

        Config()
            : history(500),
            var_threshold(16.0),
            detect_shadows(true),
            min_area(1500),
            morph_open_k(3),
            morph_close_k(7)
        {}
    };

    explicit GMMSegmenter(const Config& cfg = Config());

    cv::Mat apply(const cv::Mat& frame);

    std::vector<cv::Rect> getRegions(const cv::Mat& mask) const;

    cv::Mat getBackground() const;

private:

    Config cfg_;

    cv::Ptr<cv::BackgroundSubtractorMOG2> pMOG2_;

    cv::Mat kernel_open_;
    cv::Mat kernel_close_;
};