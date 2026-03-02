#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

void ejercicio7_vignette(int argc, char** argv, double k = 0.4) {
    std::string route = argv[1];
    Mat img_bgr = imread(route);
    if (img_bgr.empty()) return;
    int rows = img_bgr.rows;
    int cols = img_bgr.cols;
    // TODO: PASO 1 - Calcular centro de la imagen
    double cx = cols / 2.0;
    double cy = rows / 2.0;
    // TODO: PASO 2 - Calcular distancia máxima (a la esquina)
    double max_dist = sqrt(cx * cx + cy * cy);
    // TODO: PASO 3 - Aplicar corrección píxel por píxel
    Mat img_resultado = img_bgr.clone();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double dx = j - cx;
            double dy = i - cy;
            double dist = sqrt(dx * dx + dy * dy);
            double factor = 1.0 + k * (dist / max_dist);
            Vec3b pixel = img_bgr.at<Vec3b>(i, j);
            pixel[0] = saturate_cast<uchar>(pixel[0] / factor);
            pixel[1] = saturate_cast<uchar>(pixel[1] / factor);
            pixel[2] = saturate_cast<uchar>(pixel[2] / factor);
            img_resultado.at<Vec3b>(i, j) = pixel;
        }
    }
    imshow("Original", img_bgr);
    imshow("Viñeteo Corregido k=" + to_string(k), img_resultado);
    waitKey(0);
    destroyAllWindows();
}
int main(int argc, char** argv) {
        if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_imagen>\n";
        return 1;
    }
    ejercicio7_vignette(argc, argv, 1.5);
    return 0;
}