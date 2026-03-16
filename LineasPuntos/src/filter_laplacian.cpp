#include "filters.h"
#include "filter_utils.h"
#include <cmath>

cv::Mat applyLaplacianEdges(const cv::Mat& gray) {
    static const double K[9] = {0,1,0, 1,-4,1, 0,1,0};
    cv::Mat resp = applyKernel3x3(gray, K);
    for (int i = 0; i < gray.rows * gray.cols; ++i)
        resp.at<double>(i) = std::abs(resp.at<double>(i));
    return normTo8U(resp);
}
