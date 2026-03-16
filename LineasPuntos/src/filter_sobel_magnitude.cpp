#include "filters.h"
#include "filter_utils.h"
#include <cmath>

cv::Mat applySobelMagnitude(const cv::Mat& gray) {
    static const double Kx[9] = {-1,0,1, -2,0,2, -1,0,1};
    static const double Ky[9] = {-1,-2,-1, 0,0,0, 1,2,1};

    cv::Mat gx = applyKernel3x3(gray, Kx);
    cv::Mat gy = applyKernel3x3(gray, Ky);

    cv::Mat mag(gray.rows, gray.cols, CV_64F);
    for (int i = 0; i < gray.rows * gray.cols; ++i) {
        double vx = gx.at<double>(i), vy = gy.at<double>(i);
        mag.at<double>(i) = std::sqrt(vx * vx + vy * vy);
    }
    return normTo8U(mag);
}
