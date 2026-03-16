#pragma once
#include <opencv2/opencv.hpp>

// All functions receive a grayscale (CV_8UC1) frame.
// They return a CV_8UC1 (or CV_8UC3 for Hough) result ready to display.

// 1. Laplacian of Gaussian
cv::Mat applyLoG(const cv::Mat& gray, int kernelSize = 9, double sigma = 1.4);

// 2. Zero Crossing (over a LoG response)
cv::Mat applyZeroCrossing(const cv::Mat& gray, int kernelSize = 9,
                           double sigma = 1.4, int zcThreshold = 10);

// 3. Sobel (|Gx| + |Gy|, normalized)
cv::Mat applySobel(const cv::Mat& gray);

// 4. Scharr (|Gx| + |Gy|, normalized)
cv::Mat applyScharr(const cv::Mat& gray);

// 5. Laplacian Edges (discrete 3x3 kernel)
cv::Mat applyLaplacianEdges(const cv::Mat& gray);

// 6. Sobel Magnitude sqrt(Gx^2 + Gy^2)
cv::Mat applySobelMagnitude(const cv::Mat& gray);

// 7. Hough Lines — returns BGR image with lines drawn in red
cv::Mat applyHough(const cv::Mat& gray, int houghThreshold = 80);

// 8. Canny
cv::Mat applyCanny(const cv::Mat& gray, double low = 50.0, double high = 150.0);
