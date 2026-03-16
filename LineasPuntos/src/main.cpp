#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "filters.h"

// ── Converts any result (grayscale or BGR) to BGR for uniform tiling ─────────
static cv::Mat toBGR(const cv::Mat& src) {
    if (src.channels() == 1) {
        cv::Mat out;
        cv::cvtColor(src, out, cv::COLOR_GRAY2BGR);
        return out;
    }
    return src.clone();
}

// ── Builds a single tiled image from a list of (label, frame) pairs ──────────
static cv::Mat buildTile(const std::vector<std::pair<std::string, cv::Mat>>& panels,
                          int cols, int cellW, int cellH) {
    int rows = ((int)panels.size() + cols - 1) / cols;
    int borderPad = 4;
    int labelH    = 22;
    int totalCellH = cellH + labelH + borderPad * 2;
    int totalCellW = cellW + borderPad * 2;

    cv::Mat canvas(rows * totalCellH, cols * totalCellW, CV_8UC3,
                   cv::Scalar(30, 30, 30));

    for (int idx = 0; idx < (int)panels.size(); ++idx) {
        const auto& [label, img] = panels[idx];
        int row = idx / cols;
        int col = idx % cols;

        int xOff = col * totalCellW + borderPad;
        int yOff = row * totalCellH + borderPad;

        // Resize panel to cellW × cellH
        cv::Mat resized;
        cv::resize(toBGR(img), resized, cv::Size(cellW, cellH));

        // Copy into canvas
        resized.copyTo(canvas(cv::Rect(xOff, yOff, cellW, cellH)));

        // Draw label background
        cv::rectangle(canvas,
                      cv::Point(xOff, yOff + cellH),
                      cv::Point(xOff + cellW, yOff + cellH + labelH),
                      cv::Scalar(50, 50, 50), cv::FILLED);

        // Draw label text
        cv::putText(canvas, label,
                    cv::Point(xOff + 4, yOff + cellH + labelH - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.45,
                    cv::Scalar(220, 220, 220), 1, cv::LINE_AA);
    }
    return canvas;
}

int main(int argc, char* argv[]) {
    // ── Open camera (index 0, or pass index/path as argument) ────────────────
    int camIdx = 0;
    if (argc > 1) {
        try { camIdx = std::stoi(argv[1]); }
        catch (...) { camIdx = 0; }
    }

    cv::VideoCapture cap(camIdx);
    if (!cap.isOpened()) {
        std::cerr << "ERROR: Cannot open camera " << camIdx << "\n";
        std::cerr << "Usage: " << argv[0] << " [camera_index]\n";
        return 1;
    }

    // Optional: limit capture resolution for performance
    cap.set(cv::CAP_PROP_FRAME_WIDTH,  640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    std::cout << "Camera opened. Press 'q' or ESC to quit.\n";
    std::cout << "Press 'p' to pause/resume.\n";

    const std::string WIN = "LineasPuntos — 8 Filtros en Tiempo Real";
    cv::namedWindow(WIN, cv::WINDOW_NORMAL);

    // Grid: 3 cols × 3 rows = 9 panels (original + 8 filters)
    const int GRID_COLS = 3;
    const int CELL_W    = 213;
    const int CELL_H    = 160;

    cv::Mat frame, gray;
    bool paused = false;
    cv::Mat lastCanvas;

    while (true) {
        if (!paused) {
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "Empty frame, retrying...\n";
                cv::waitKey(30);
                continue;
            }

            // ── Convert to grayscale ─────────────────────────────────────────
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            // ── Apply all filters ────────────────────────────────────────────
            cv::Mat resGray;
            cv::resize(gray, resGray, cv::Size(CELL_W, CELL_H)); // for original

            cv::Mat r1  = applyLoG(gray, 9, 1.4);
            cv::Mat r2  = applyZeroCrossing(gray, 9, 1.4, 10);
            cv::Mat r3  = applySobel(gray);
            cv::Mat r4  = applyScharr(gray);
            cv::Mat r5  = applyLaplacianEdges(gray);
            cv::Mat r6  = applySobelMagnitude(gray);
            cv::Mat r7  = applyHough(gray, 80);
            cv::Mat r8  = applyCanny(gray, 50.0, 150.0);

            // ── Build 3×3 tile grid ──────────────────────────────────────────
            std::vector<std::pair<std::string, cv::Mat>> panels = {
                {"Original (Gray)",     gray},
                {"1. LoG",              r1},
                {"2. Zero Crossing",    r2},
                {"3. Sobel |Gx|+|Gy|",  r3},
                {"4. Scharr",           r4},
                {"5. Laplacian Edges",  r5},
                {"6. Sobel Magnitude",  r6},
                {"7. Hough Lines",      r7},
                {"8. Canny",            r8},
            };

            lastCanvas = buildTile(panels, GRID_COLS, CELL_W, CELL_H);
        }

        cv::imshow(WIN, lastCanvas);

        int key = cv::waitKey(1) & 0xFF;
        if (key == 'q' || key == 27) break;       // q or ESC → quit
        if (key == 'p' || key == ' ') {            // p or space → pause
            paused = !paused;
            std::cout << (paused ? "Paused.\n" : "Resumed.\n");
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
