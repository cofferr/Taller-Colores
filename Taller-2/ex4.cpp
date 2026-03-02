#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

void ejercicio4_gray_world(int argc, char** argv) {
    std::string route = argv[1];
    Mat img_bgr = imread(route);
    if (img_bgr.empty()) return;
    int rows = img_bgr.rows;
    int cols = img_bgr.cols;
    int total_pixels = rows * cols;
    // TODO: PASO 1 - Calcular suma de cada canal
    double sum_b = 0, sum_g = 0, sum_r = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec3b pixel = img_bgr.at<Vec3b>(i, j);
            sum_b += pixel[0];
            sum_g += pixel[1];
            sum_r += pixel[2];
        }
    }
    // TODO: PASO 2 - Calcular promedios
    double avg_b = sum_b / total_pixels;
    double avg_g = sum_g / total_pixels;
    double avg_r = sum_r / total_pixels;
    // TODO: PASO 3 - Calcular promedio gris
    double avg_gray = (avg_b + avg_g + avg_r) / 3.0;
    // TODO: PASO 4 - Calcular factores de escala
    double scale_b = avg_gray / avg_b;
    double scale_g = avg_gray / avg_g;
    double scale_r = avg_gray / avg_r;

    Mat img_resultado = img_bgr.clone();  
    // TODO: PASO 5 - Crear imagen corregida
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec3b pixel = img_bgr.at<Vec3b>(i, j);
            Vec3b corrected_pixel(
                saturate_cast<uchar>(pixel[0] * scale_b),
                saturate_cast<uchar>(pixel[1] * scale_g),
                saturate_cast<uchar>(pixel[2] * scale_r)
            );
            img_resultado.at<Vec3b>(i, j) = corrected_pixel;
        }
    }

    imshow("Original", img_bgr);
    imshow("Gray World", img_resultado);
    waitKey(0);
    destroyAllWindows();
}
int main(int argc, char** argv) {
        if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_imagen>\n";
        return 1;
    }
    ejercicio4_gray_world(argc, argv);
    return 0;
}