#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;
void ejercicio6_gamma(int argc, char** argv, double gamma = 1.5) {
    std::string route = argv[1];
    Mat img_bgr = imread(route);
    if (img_bgr.empty()) return;
    int rows = img_bgr.rows;
    int cols = img_bgr.cols;
    // TODO: PASO 1 - Crear tabla de lookup (para eficiencia)
    // Pre-calcular la transformación para todos los valores 0-255
    vector<uchar> lut(256);
    for (int i = 0; i < 256; i++) {
        double normalized = i / 255.0;
        double corrected = 255.0 * pow(normalized, gamma);
        lut[i] = saturate_cast<uchar>(corrected);
    }
    // TODO: PASO 2 - Aplicar transformación a cada píxel
    Mat img_resultado = img_bgr.clone();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            Vec3b pixel = img_bgr.at<Vec3b>(i, j);

            pixel[0] = lut[pixel[0]];
            pixel[1] = lut[pixel[1]];
            pixel[2] = lut[pixel[2]];

            img_resultado.at<Vec3b>(i, j) = pixel;
        }
    }
    imshow("Original", img_bgr);
    imshow("Gamma = " + to_string(gamma), img_resultado);
    waitKey(0);
    destroyAllWindows();
}
int main(int argc, char** argv) {
        if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_imagen>\n";
        return 1;
    }
    ejercicio6_gamma(argc, argv, 1.5);
    return 0;
}