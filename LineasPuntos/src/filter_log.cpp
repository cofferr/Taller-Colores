#include "filters.h"
#include "filter_utils.h"
#include <cmath>

cv::Mat applyLoG(const cv::Mat& gray, int kernelSize, double sigma) {
    if (kernelSize % 2 == 0) kernelSize++;
    int half = kernelSize / 2;
    int W = gray.cols, H = gray.rows;

    // Build LoG kernel analytically
    cv::Mat kernel(kernelSize, kernelSize, CV_64F);
    double s2 = sigma * sigma;
    for (int y = -half; y <= half; ++y)
        for (int x = -half; x <= half; ++x) {
            double r2 = (double)(x * x + y * y);
            kernel.at<double>(y + half, x + half) =
                -(1.0 / (PI * s2 * s2)) *
                (1.0 - r2 / (2.0 * s2)) *
                std::exp(-r2 / (2.0 * s2));
        }

    // Convolve manually
    cv::Mat resp(H, W, CV_64F, 0.0);
    for (int r = half; r < H - half; ++r)
        for (int c = half; c < W - half; ++c) {
            double acc = 0.0;
            for (int ky = -half; ky <= half; ++ky)
                for (int kx = -half; kx <= half; ++kx)
                    acc += kernel.at<double>(ky + half, kx + half) *
                           (double)gray.at<uchar>(r + ky, c + kx);
            resp.at<double>(r, c) = acc;
        }

    return normTo8U(resp);
}
