#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;
struct Pixel {
    double r, g, b;
    Pixel() : r(0), g(0), b(0) {}
    Pixel(double r_, double g_, double b_) : r(r_), g(g_), b(b_) {}
};
double distancia_euclidiana(const Pixel& p1, const Pixel& p2) {
    // TODO: Implementar;
    double b = p1.b - p2.b;
    double g = p1.g - p2.g;
    double r = p1.r - p2.r;
    return sqrt(r*r + g*g + b*b);
}
void ejercicio3_kmeans_manual(int argc, char** argv,int K = 5) {
    std::string route = argv[1];
    Mat img_bgr = imread(route);
    if (img_bgr.empty()) return;
    // Redimensionar para acelerar
    Mat img_small;
    resize(img_bgr, img_small, Size(160, 120));
    int rows = img_small.rows;
    int cols = img_small.cols;
    int total_pixels = rows * cols;
    cout << "Procesando " << total_pixels << " píxeles con K=" << K << endl;
    // TODO: PASO 1 - Crear array de píxeles
    vector<Pixel> pixels;
    // Almacenar todos los píxeles en un vector
    pixels.reserve(total_pixels);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            Vec3b color = img_small.at<Vec3b>(i, j);

            double b = color[0];
            double g = color[1];
            double r = color[2];

            pixels.push_back(Pixel(r, g, b));
        }
    }
    // TODO: PASO 2 - Inicializar K centroides aleatorios
    srand(time(0));

    vector<Pixel> centroids;
    for (int k = 0; k < K; k++) {
        int random_index = rand() % total_pixels;
        centroids.push_back(pixels[random_index]);
    }
    // TODO: PASO 3 - Array para almacenar asignaciones
    // Cada píxel se asigna a un cluster [0, K-1]
    vector<int> assignments(total_pixels, -1);
    // TODO: PASO 4 - Iterar K-Means
    int max_iteraciones = 20;
    for (int iter = 0; iter < max_iteraciones; iter++) {
    cout << "Iteración " << (iter + 1) << "/" << max_iteraciones << endl;
    // PASO 4a: Asignar cada píxel al centroide más cercano
    for (int i = 0; i < total_pixels; i++) {
        double min_distance = distancia_euclidiana(pixels[i], centroids[0]);
        int cluster = 0;
        for (int k = 1; k < K; k++) {
            double dist = distancia_euclidiana(pixels[i], centroids[k]);
            if (dist < min_distance) {
                min_distance = dist;
                cluster = k;
            }
        }
        assignments[i] = cluster;
    }
    // PASO 4b: Recalcular centroides
    // Crear arrays para sumar RGB de cada cluster
    vector<Pixel> new_centroids(K);
    vector<int> counts(K, 0);
    for (int i = 0; i < total_pixels; i++) {
        int cluster = assignments[i];
        new_centroids[cluster].r += pixels[i].r;
        new_centroids[cluster].g += pixels[i].g;
        new_centroids[cluster].b += pixels[i].b;
        counts[cluster]++;
    }
    for (int k = 0; k < K; k++) {
        if (counts[k] > 0) {
            centroids[k].r = new_centroids[k].r / counts[k];
            centroids[k].g = new_centroids[k].g / counts[k];
            centroids[k].b = new_centroids[k].b / counts[k];
        }
    }
    }
    // TODO: PASO 5 - Crear imagen cuantizada
    // Reemplazar cada píxel por el color de su centroide
    Mat img_quantized = img_small.clone();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            int cluster = assignments[index];
            Vec3b color(centroids[cluster].b, centroids[cluster].g, centroids[cluster].r);
            img_quantized.at<Vec3b>(i, j) = color;
        }
    }

    // TODO: PASO 6 - Crear paleta de colores
    Mat paleta(50, K * 50, CV_8UC3);
    for (int k = 0; k < K; k++) {
        Vec3b color(centroids[k].b, centroids[k].g, centroids[k].r);
        for (int y = 0; y < 50; y++) {
            for (int x = k * 50; x < (k + 1) * 50; x++) {
                paleta.at<Vec3b>(y, x) = color;
            }
        }
    }
    imshow("Original", img_small);
    imshow("K-Means Manual K=" + to_string(K), img_quantized);
    imshow("Paleta", paleta);
    waitKey(0);
    destroyAllWindows();
}
int main(int argc, char** argv) {

    if (argc < 3) {
        cout << "Uso: programa imagen.jpg" << endl;
        return -1;
    }
    int K = atoi(argv[2]);
    ejercicio3_kmeans_manual(argc, argv, K);

    return 0;
}