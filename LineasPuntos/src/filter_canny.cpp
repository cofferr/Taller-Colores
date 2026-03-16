#include "filters.h"
#include "filter_utils.h"
#include <cmath>

cv::Mat applyCanny(const cv::Mat& gray, double low, double high) {
    int W = gray.cols, H = gray.rows;

    // Step 1: Gaussian blur
    cv::Mat blurF = manualGaussianBlur(gray, 5, 1.0);

    // Step 2: Sobel gradients
    static const double Kx[9] = {-1,0,1, -2,0,2, -1,0,1};
    static const double Ky[9] = {-1,-2,-1, 0,0,0, 1,2,1};

    cv::Mat gx = applyKernel3x3(blurF, Kx);
    cv::Mat gy = applyKernel3x3(blurF, Ky);

    cv::Mat mag(H, W, CV_64F), dir(H, W, CV_64F);
    for (int i = 0; i < H * W; ++i) {
        double vx = gx.at<double>(i), vy = gy.at<double>(i);
        mag.at<double>(i) = std::sqrt(vx * vx + vy * vy);
        dir.at<double>(i) = std::atan2(vy, vx) * 180.0 / PI;
    }

    // Step 3: Non-maximum suppression
    cv::Mat nms(H, W, CV_64F, 0.0);
    for (int r = 1; r < H - 1; ++r)
        for (int c = 1; c < W - 1; ++c) {
            double angle = dir.at<double>(r, c);
            if (angle < 0) angle += 180.0;

            double q, p, m = mag.at<double>(r, c);
            if (angle < 22.5 || angle >= 157.5) {
                q = mag.at<double>(r, c + 1); p = mag.at<double>(r, c - 1);
            } else if (angle < 67.5) {
                q = mag.at<double>(r + 1, c - 1); p = mag.at<double>(r - 1, c + 1);
            } else if (angle < 112.5) {
                q = mag.at<double>(r + 1, c); p = mag.at<double>(r - 1, c);
            } else {
                q = mag.at<double>(r - 1, c - 1); p = mag.at<double>(r + 1, c + 1);
            }
            nms.at<double>(r, c) = (m >= q && m >= p) ? m : 0.0;
        }

    // Step 4: Double threshold (normalize to 0-255 first)
    double maxMag; cv::minMaxLoc(nms, nullptr, &maxMag);
    double scale = (maxMag > 0) ? 255.0 / maxMag : 1.0;

    enum : uchar { NONE = 0, WEAK = 128, STRONG = 255 };
    cv::Mat edge(H, W, CV_8U, cv::Scalar(NONE));
    for (int i = 0; i < H * W; ++i) {
        double v = nms.at<double>(i) * scale;
        if (v >= high)      edge.at<uchar>(i) = STRONG;
        else if (v >= low)  edge.at<uchar>(i) = WEAK;
    }

    // Step 5: Hysteresis
    bool changed = true;
    while (changed) {
        changed = false;
        for (int r = 1; r < H - 1; ++r)
            for (int c = 1; c < W - 1; ++c) {
                if (edge.at<uchar>(r, c) != WEAK) continue;
                bool hasStrong = false;
                for (int dy = -1; dy <= 1 && !hasStrong; ++dy)
                    for (int dx = -1; dx <= 1 && !hasStrong; ++dx)
                        if (edge.at<uchar>(r + dy, c + dx) == STRONG)
                            hasStrong = true;
                if (hasStrong) { edge.at<uchar>(r, c) = STRONG; changed = true; }
            }
    }
    for (int i = 0; i < H * W; ++i)
        if (edge.at<uchar>(i) == WEAK) edge.at<uchar>(i) = NONE;

    return edge;
}
