#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

inline constexpr double PI = 3.14159265358979323846;

// Build a normalized Gaussian 1-D kernel of size (2*halfSize+1).
std::vector<double> gaussianKernel1D(int halfSize, double sigma);

// Separable Gaussian blur — returns CV_64F.
cv::Mat manualGaussianBlur(const cv::Mat& src, int kernelSize, double sigma);

// Apply a flat 3×3 double kernel — returns CV_64F.
cv::Mat applyKernel3x3(const cv::Mat& src, const double k[9]);

// Normalize CV_64F → CV_8UC1 in [0, 255].
cv::Mat normTo8U(const cv::Mat& src);
