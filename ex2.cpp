#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;
void ejercicio2_modificar_saturacion(int argc, char** argv) {
    std::string route = argv[1];
    Mat img_bgr = imread(route);
    int rows = img_bgr.rows;
    int cols = img_bgr.cols;
    Mat img_hsv(rows, cols, CV_8UC3);
    // TODO: Copiar código de conversión BGR→HSV aquí
    // ... (del ejercicio 1)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
        Vec3b pixel_bgr = img_bgr.at<Vec3b>(i, j);
        // Obtener valores BGR y normalizar
        double b = pixel_bgr[0] / 255.0;
        double g = pixel_bgr[1] / 255.0;
        double r = pixel_bgr[2] / 255.0;
        // Calcular Cmax, Cmin, Delta
            double cmax = max ({r,g,b});
            double cmin = min ({r,g,b});
            double delta = cmax - cmin;
        // Calcular Hue (H)
            double hue = 0.0;
            if (delta == 0) {
                hue = 0;
            } else if (cmax == r){
                hue = 60 * fmod(((g-b)/delta),6);
            } else if (cmax == g){
                hue = 60 * ((b-r)/delta + 2);
            } else if (cmax == b){
                hue = 60 * ((r-g)/delta + 4);
            }
            double hue_opencv = hue / 2;
        // Calcular Saturation (S)
        double saturation = 0.0;
        if (cmax == 0){
            saturation = 0;
        } else {
            saturation = delta / cmax;
        }
        double saturation_opencv = saturation * 255;
        // Calcular Value (V)
        double value = cmax;
        double value_opencv = value * 255;
        // Asignar valores HSV al píxel
        img_hsv.at<Vec3b>(i, j) = Vec3b(hue_opencv, saturation_opencv, value_opencv);

        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
        Vec3b pixel_hsv = img_hsv.at<Vec3b>(i, j);
        // PASO 1: Obtener valores H, S, V
            double hue_opencv = pixel_hsv[0];
            double saturation_opencv = pixel_hsv[1];
            double value_opencv = pixel_hsv[2];

        // PASO 2: Multiplicar S por 1.5 (sin exceder 255)
            double new_saturation = saturation_opencv * 1.5;
            if (new_saturation > 255){
                new_saturation = 255;
            }
        // PASO 3: Asignar nuevos valores
        img_hsv.at<Vec3b>(i, j) = Vec3b(hue_opencv, new_saturation, value_opencv);
        }
    }
    Mat img_resultado(rows, cols, CV_8UC3);
    // TODO: Implementar conversión HSV → BGR
    // Usa las fórmulas inversas explicadas en la teoría
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
        Vec3b pixel_hsv = img_hsv.at<Vec3b>(i, j);
        // Obtener valores H, S, V
            double hue_opencv = pixel_hsv[0];
            double saturation_opencv = pixel_hsv[1];
            double value_opencv = pixel_hsv[2];
        // Convertir H de OpenCV a grados
            double hue = hue_opencv * 2;
        // Normalizar S y V
            double saturation = saturation_opencv / 255.0;
            double value = value_opencv / 255.0;
        // Calcular C, X, m
            double c = value * saturation;
            double x = c * (1 - abs(fmod(hue / 60.0, 2) - 1));
            double m = value - c;
        // Determinar R', G', B'
            double r_prime, g_prime, b_prime;
            if (hue >= 0 && hue < 60) {
                r_prime = c; g_prime = x; b_prime = 0;
            } else if (hue >= 60 && hue < 120) {
                r_prime = x; g_prime = c; b_prime = 0;
            } else if (hue >= 120 && hue < 180) {
                r_prime = 0; g_prime = c; b_prime = x;
            } else if (hue >= 180 && hue < 240) {
                r_prime = 0; g_prime = x; b_prime = c;
            } else if (hue >= 240 && hue < 300) {
                r_prime = x; g_prime = 0; b_prime = c;
            } else {
                r_prime = c; g_prime = 0; b_prime = x;
            }
        // Convertir a BGR y asignar al píxel
            uchar b_final = static_cast<uchar>((b_prime + m) * 255);
            uchar g_final = static_cast<uchar>((g_prime + m) * 255);
            uchar r_final = static_cast<uchar>((r_prime + m) * 255);
        img_resultado.at<Vec3b>(i, j) = Vec3b(b_final, g_final, r_final);
        }
    }
    imshow("Original", img_bgr);
    imshow("Saturación Aumentada", img_resultado);
    waitKey(0);
    destroyAllWindows();
}
int main(int argc, char** argv) {
        if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_imagen>\n";
        return 1;
    }
    ejercicio2_modificar_saturacion(argc, argv);
    return 0;
}