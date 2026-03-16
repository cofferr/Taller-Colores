#include "filter_utils.h"
#include <cmath>
#include <algorithm>

std::vector<double> gaussianKernel1D(int halfSize, double sigma) {
    int size = 2 * halfSize + 1;
    std::vector<double> k(size);
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        double x = i - halfSize;
        k[i] = std::exp(-(x * x) / (2.0 * sigma * sigma));
        sum += k[i];
    }
    for (auto& v : k) v /= sum;
    return k;
}

cv::Mat manualGaussianBlur(const cv::Mat& src, int kernelSize, double sigma) {
    if (kernelSize % 2 == 0) kernelSize++;
    int half = kernelSize / 2;
    int W = src.cols, H = src.rows;

    auto k = gaussianKernel1D(half, sigma);

    // Horizontal pass → double buffer
    cv::Mat tmp(H, W, CV_64F, 0.0);
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c) {
            double acc = 0.0;
            for (int i = -half; i <= half; ++i) {
                int cc = std::clamp(c + i, 0, W - 1);
                acc += k[i + half] * src.at<uchar>(r, cc);
            }
            tmp.at<double>(r, c) = acc;
        }

    // Vertical pass
    cv::Mat out(H, W, CV_64F, 0.0);
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c) {
            double acc = 0.0;
            for (int i = -half; i <= half; ++i) {
                int rr = std::clamp(r + i, 0, H - 1);
                acc += k[i + half] * tmp.at<double>(rr, c);
            }
            out.at<double>(r, c) = acc;
        }
    return out; // CV_64F
}

cv::Mat applyKernel3x3(const cv::Mat& src, const double k[9]) {
    int W = src.cols, H = src.rows;
    cv::Mat out(H, W, CV_64F, 0.0);
    for (int r = 1; r < H - 1; ++r)
        for (int c = 1; c < W - 1; ++c) {
            double acc = 0.0;
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx) {
                    double px = (src.type() == CV_64F)
                        ? src.at<double>(r + dy, c + dx)
                        : (double)src.at<uchar>(r + dy, c + dx);
                    acc += k[(dy + 1) * 3 + (dx + 1)] * px;
                }
            out.at<double>(r, c) = acc;
        }
    return out;
}

cv::Mat normTo8U(const cv::Mat& src) {
    cv::Mat out;
    cv::normalize(src, out, 0, 255, cv::NORM_MINMAX, CV_8U);
    return out;
}
