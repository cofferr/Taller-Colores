#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;
void ejercicio1_rgb_a_hsv(int argc, char** argv) {
    std::string route = argv[1];
    Mat img_bgr = imread(route);
    if (img_bgr.empty()) {
    cout << "Error: No se pudo cargar la imagen" << endl;
    return;
    }
    int rows = img_bgr.rows;
    int cols = img_bgr.cols;
    Mat img_hsv(rows, cols, CV_8UC3);
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
    imshow("Original BGR", img_bgr);
    imshow("HSV (Manual)", img_hsv);
    waitKey(0);
    destroyAllWindows();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_imagen>\n";
        return 1;
    }
    ejercicio1_rgb_a_hsv(argc, argv);
    return 0;
}