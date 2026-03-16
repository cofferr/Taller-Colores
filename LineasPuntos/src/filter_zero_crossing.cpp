#include "filters.h"
#include "filter_utils.h"
#include <cmath>

cv::Mat applyZeroCrossing(const cv::Mat& gray, int kernelSize,
                           double sigma, int zcThreshold) {
    cv::Mat logImg = applyLoG(gray, kernelSize, sigma);
    int W = logImg.cols, H = logImg.rows;

    // Shift to signed around 128
    cv::Mat out(H, W, CV_8U, cv::Scalar(0));
    for (int r = 1; r < H - 1; ++r)
        for (int c = 1; c < W - 1; ++c) {
            int v = (int)logImg.at<uchar>(r, c) - 128;
            int neighbors[4] = {
                (int)logImg.at<uchar>(r - 1, c) - 128,
                (int)logImg.at<uchar>(r + 1, c) - 128,
                (int)logImg.at<uchar>(r, c - 1) - 128,
                (int)logImg.at<uchar>(r, c + 1) - 128
            };
            for (int n : neighbors) {
                if (v * n < 0 && std::abs(v - n) >= zcThreshold) {
                    out.at<uchar>(r, c) = 255;
                    break;
                }
            }
        }
    return out;
}
