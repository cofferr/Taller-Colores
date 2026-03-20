#include <opencv2/opencv.hpp>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>

struct Point2i { int x, y; };

static Point2i pivot_gs;

long long cross(const Point2i& O, const Point2i& A, const Point2i& B)
{
    return (long long)(A.x - O.x) * (B.y - O.y)
         - (long long)(A.y - O.y) * (B.x - O.x);
}

double dist2(const Point2i& a, const Point2i& b)
{
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx*dx + dy*dy;
}

std::vector<Point2i> grahamScan(std::vector<Point2i> pts)
{
    int n = static_cast<int>(pts.size());
    if (n < 3) return pts;

    int pivot = 0;
    for (int i = 1; i < n; ++i)
        if (pts[i].y < pts[pivot].y ||
           (pts[i].y == pts[pivot].y && pts[i].x < pts[pivot].x))
            pivot = i;

    std::swap(pts[0], pts[pivot]);
    pivot_gs = pts[0];

    std::sort(pts.begin() + 1, pts.end(), [](const Point2i& a, const Point2i& b) {
        long long cr = cross(pivot_gs, a, b);
        if (cr != 0) return cr > 0;
        return dist2(pivot_gs, a) < dist2(pivot_gs, b);
    });

    std::vector<Point2i> sorted;
    sorted.push_back(pts[0]);
    for (int i = 1; i < n; ) {
        int j = i;
        while (j < n - 1 && cross(pivot_gs, pts[j], pts[j+1]) == 0) ++j;
        sorted.push_back(pts[j]);
        i = j + 1;
    }

    if (static_cast<int>(sorted.size()) < 3) return sorted;

    std::vector<Point2i> hull;
    hull.push_back(sorted[0]);
    hull.push_back(sorted[1]);

    for (int i = 2; i < static_cast<int>(sorted.size()); ++i) {
        while (hull.size() > 1 &&
               cross(hull[hull.size()-2], hull[hull.size()-1], sorted[i]) <= 0)
            hull.pop_back();
        hull.push_back(sorted[i]);
    }
    return hull;
}

std::vector<Point2i> edgePixels(const cv::Mat& edgeBin)
{
    std::vector<Point2i> pts;
    for (int r = 0; r < edgeBin.rows; ++r)
        for (int c = 0; c < edgeBin.cols; ++c)
            if (edgeBin.at<uchar>(r, c) > 0)
                pts.push_back({c, r});
    return pts;
}

void drawHull(cv::Mat& img, const std::vector<Point2i>& hull)
{
    if (hull.empty()) return;
    int n = static_cast<int>(hull.size());
    for (int i = 0; i < n; ++i) {
        cv::Point p1(hull[i].x, hull[i].y);
        cv::Point p2(hull[(i+1) % n].x, hull[(i+1) % n].y);
        cv::line(img, p1, p2, cv::Scalar(0, 255, 0), 2);
        cv::circle(img, p1, 4, cv::Scalar(0, 0, 255), -1);
    }
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: no se pudo abrir la cámara.\n";
        return 1;
    }

    int  thresh       = 127;
    bool showPipeline = false;

    std::cout << "Controles:\n"
              << "  '+'/'-'  → subir/bajar umbral (" << thresh << ")\n"
              << "  'p'      → mostrar/ocultar pipeline\n"
              << "  'q'      → salir\n";

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // ── Pipeline OpenCV ──────────────────────────────────────
        cv::Mat gray, bin, lapRaw, lapAbs, edges;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::threshold(gray, bin, thresh, 255, cv::THRESH_BINARY);
        cv::Laplacian(bin, lapRaw, CV_16S, 3);
        cv::convertScaleAbs(lapRaw, lapAbs);
        cv::threshold(lapAbs, edges, 10, 255, cv::THRESH_BINARY);

        auto pts  = edgePixels(edges);
        auto hull = grahamScan(pts);

        cv::Mat result = frame.clone();
        drawHull(result, hull);

        // HUD
        cv::putText(result, "Umbral: " + std::to_string(thresh),
                    {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.8, {255,255,0}, 2);

        if (showPipeline) {
            // Convertir grises→BGR para apilar
            cv::Mat grayBGR, binBGR, edgesBGR;
            cv::cvtColor(gray,  grayBGR,  cv::COLOR_GRAY2BGR);
            cv::cvtColor(bin,   binBGR,   cv::COLOR_GRAY2BGR);
            cv::cvtColor(edges, edgesBGR, cv::COLOR_GRAY2BGR);

            int h = frame.rows, w = frame.cols;
            cv::Mat top, bottom, grid;
            cv::hconcat(grayBGR, binBGR,   top);
            cv::hconcat(edgesBGR, result,  bottom);
            cv::vconcat(top, bottom, grid);
            cv::resize(grid, grid, {w, h});

            cv::putText(grid, "Grises",         {5,           15}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0,255,255}, 1);
            cv::putText(grid, "Mascara Binaria",{w/2+5,       15}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0,255,255}, 1);
            cv::putText(grid, "Bordes",         {5,           h/2+15}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0,255,255}, 1);
            cv::putText(grid, "Convex Hull",    {w/2+5,       h/2+15}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0,255,255}, 1);

            cv::imshow("Pipeline - Proceso 1 (Threshold)", grid);
        } else {
            cv::imshow("Convex Hull - Proceso 1 (Threshold)", result);
        }

        int key = cv::waitKey(1) & 0xFF;
        if      (key == 'q')              break;
        else if (key == '+' || key == '=') thresh = std::min(255, thresh + 5);
        else if (key == '-')               thresh = std::max(0,   thresh - 5);
        else if (key == 'p') {
            showPipeline = !showPipeline;
            cv::destroyAllWindows();
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
