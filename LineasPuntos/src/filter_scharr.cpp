#include "filters.h"
#include "filter_utils.h"
#include <cmath>

cv::Mat applyScharr(const cv::Mat& gray) {
    static const double Kx[9] = {-3,0,3, -10,0,10, -3,0,3};
    static const double Ky[9] = {-3,-10,-3, 0,0,0, 3,10,3};

    cv::Mat gx = applyKernel3x3(gray, Kx);
    cv::Mat gy = applyKernel3x3(gray, Ky);

    cv::Mat mag(gray.rows, gray.cols, CV_64F);
    for (int i = 0; i < gray.rows * gray.cols; ++i)
        mag.at<double>(i) = std::abs(gx.at<double>(i)) +
                             std::abs(gy.at<double>(i));
    return normTo8U(mag);
}
