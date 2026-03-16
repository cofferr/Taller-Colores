#include "filters.h"
#include "filter_utils.h"
#include <cmath>
#include <vector>

cv::Mat applyHough(const cv::Mat& gray, int houghThreshold) {
    int W = gray.cols, H = gray.rows;

    // Use Canny edges internally as input
    cv::Mat edges = applyCanny(gray, 50.0, 150.0);

    int diagLen = (int)std::ceil(std::sqrt((double)(W * W + H * H)));
    int numRho   = 2 * diagLen + 1;
    int numTheta = 180;

    std::vector<double> cosT(numTheta), sinT(numTheta);
    for (int t = 0; t < numTheta; ++t) {
        double angle = (double)t * PI / numTheta;
        cosT[t] = std::cos(angle);
        sinT[t] = std::sin(angle);
    }

    // Accumulator
    std::vector<int> accum(numRho * numTheta, 0);
    for (int r = 0; r < H; ++r)
        for (int c = 0; c < W; ++c) {
            if (edges.at<uchar>(r, c) == 0) continue;
            for (int t = 0; t < numTheta; ++t) {
                int rho = (int)std::round(c * cosT[t] + r * sinT[t]) + diagLen;
                if (rho >= 0 && rho < numRho)
                    accum[rho * numTheta + t]++;
            }
        }

    // Output: color BGR image with lines drawn
    cv::Mat colorOut;
    cv::cvtColor(gray, colorOut, cv::COLOR_GRAY2BGR);

    for (int rhoIdx = 0; rhoIdx < numRho; ++rhoIdx) {
        for (int t = 0; t < numTheta; ++t) {
            if (accum[rhoIdx * numTheta + t] < houghThreshold) continue;

            double rho   = (double)(rhoIdx - diagLen);
            double angle = (double)t * PI / numTheta;
            double ct = std::cos(angle), st = std::sin(angle);

            // Find two extreme points on the line
            cv::Point pt1, pt2;
            if (std::abs(st) > 1e-6) {
                pt1 = cv::Point(0,        (int)std::round(rho / st));
                pt2 = cv::Point(W - 1,    (int)std::round((rho - (W-1)*ct) / st));
            } else {
                pt1 = cv::Point((int)std::round(rho / ct), 0);
                pt2 = cv::Point((int)std::round(rho / ct), H - 1);
            }
            cv::line(colorOut, pt1, pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
        }
    }
    return colorOut; // CV_8UC3
}
