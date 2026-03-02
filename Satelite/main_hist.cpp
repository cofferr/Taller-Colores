#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
    if(argc < 2){
        cout << "Uso: " << argv[0] << " <ruta_imagen> [save]" << endl;
        cout << "  - <ruta_imagen>: ruta a la imagen a procesar" << endl;
        cout << "  - [save]: opcional, guardar todas las imagenes generadas" << endl;
        return -1;
    }

    bool saveImages = (argc >= 3 && string(argv[2]) == "save");
    
    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    if(img.empty()){
        cout << "Error cargando imagen: " << argv[1] << endl;
        return -1;
    }

    imshow("Imagen Original", img);
    if(saveImages) imwrite("01_imagen_original.png", img);

    cout << "Filas: "        << img.rows       << endl;
    cout << "Columnas: "     << img.cols       << endl;
    cout << "Canales: "      << img.channels() << endl;
    cout << "Profundidad: "  << img.depth()    << endl;

    // -------------------------------------------------------
    // HISTOGRAMA
    // -------------------------------------------------------
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    Mat hist;
    calcHist(&img, 1, 0, Mat(), hist, 1, &histSize, &histRange);

    int histH = 400, histW = 512;
    Mat histImg(histH, histW, CV_8UC1, Scalar(255));
    normalize(hist, hist, 0, histH, NORM_MINMAX);

    int binW = histW / histSize;
    for(int i = 1; i < histSize; i++){
        line(histImg,
             Point(binW*(i-1), histH - cvRound(hist.at<float>(i-1))),
             Point(binW*i,     histH - cvRound(hist.at<float>(i))),
             Scalar(0), 2);
    }
    imshow("Histograma", histImg);
    if(saveImages) imwrite("02_histograma.png", histImg);

    double minVal, maxVal;
    minMaxLoc(img, &minVal, &maxVal);
    cout << "\n--- Analisis del Histograma ---" << endl;
    cout << "Valor minimo de pixel: " << minVal << endl;
    cout << "Valor maximo de pixel: " << maxVal << endl;
    cout << "Rango dinamico: "        << (maxVal - minVal) << endl;

    if((maxVal - minVal) > 150)
        cout << "-> La imagen TIENE alto contraste." << endl;
    else
        cout << "-> La imagen NO tiene alto contraste." << endl;

    Scalar media, desviacion;
    meanStdDev(img, media, desviacion);

    cout << "\n--- Modelado Gaussiano ---" << endl;
    cout << "Media: "      << media[0]      << endl;
    cout << "Desviacion: " << desviacion[0] << endl;

    if(media[0] < 85)
        cout << "-> Media baja: imagen con predominancia de tonos OSCUROS." << endl;
    else if(media[0] > 170)
        cout << "-> Media alta: imagen con predominancia de tonos CLAROS." << endl;
    else
        cout << "-> Media media: imagen con tonos MIXTOS." << endl;

    Mat mascara2sigma = Mat::zeros(img.size(), CV_8U);
    for(int i = 0; i < img.rows; i++){
        for(int j = 0; j < img.cols; j++){
            uchar pixel = img.at<uchar>(i,j);
            if(pixel > media[0] - 2*desviacion[0] &&
               pixel < media[0] + 2*desviacion[0]){
                mascara2sigma.at<uchar>(i,j) = 255;
            }
        }
    }
    imshow("Segmentacion Gaussiana 2-sigma", mascara2sigma);
    if(saveImages) imwrite("03_segmentacion_2sigma.png", mascara2sigma);

    Mat mascara1sigma = Mat::zeros(img.size(), CV_8U);
    for(int i = 0; i < img.rows; i++){
        for(int j = 0; j < img.cols; j++){
            uchar pixel = img.at<uchar>(i,j);
            if(pixel > media[0] - 1*desviacion[0] &&
               pixel < media[0] + 1*desviacion[0]){
                mascara1sigma.at<uchar>(i,j) = 255;
            }
        }
    }
    imshow("Segmentacion Gaussiana 1-sigma", mascara1sigma);
    if(saveImages) imwrite("04_segmentacion_1sigma.png", mascara1sigma);

    Mat mascara3sigma = Mat::zeros(img.size(), CV_8U);
    for(int i = 0; i < img.rows; i++){
        for(int j = 0; j < img.cols; j++){
            uchar pixel = img.at<uchar>(i,j);
            if(pixel > media[0] - 3*desviacion[0] &&
               pixel < media[0] + 3*desviacion[0]){
                mascara3sigma.at<uchar>(i,j) = 255;
            }
        }
    }
    imshow("Segmentacion Gaussiana 3-sigma", mascara3sigma);
    if(saveImages) imwrite("05_segmentacion_3sigma.png", mascara3sigma);

    int pixeles2s = countNonZero(mascara2sigma);
    int pixeles1s = countNonZero(mascara1sigma);
    int pixeles3s = countNonZero(mascara3sigma);
    int total     = img.rows * img.cols;

    cout << "\n--- Segmentacion Gaussiana ---" << endl;
    cout << "Pixeles fondo con 1-sigma: " << pixeles1s << " (" << 100.0*pixeles1s/total << "%)" << endl;
    cout << "Pixeles fondo con 2-sigma: " << pixeles2s << " (" << 100.0*pixeles2s/total << "%)" << endl;
    cout << "Pixeles fondo con 3-sigma: " << pixeles3s << " (" << 100.0*pixeles3s/total << "%)" << endl;

    Mat suavizada;
    GaussianBlur(img, suavizada, Size(7,7), 1.5);
    imshow("Suavizada Gaussiana", suavizada);
    if(saveImages) imwrite("06_suavizada_gaussiana.png", suavizada);

    Scalar media2, desv2;
    meanStdDev(suavizada, media2, desv2);

    cout << "\n--- Filtro de Suavizado ---" << endl;
    cout << "Desviacion original:  " << desviacion[0] << endl;
    cout << "Desviacion suavizada: " << desv2[0]      << endl;

    double reduccionVar = 100.0 * (1.0 - (desv2[0]*desv2[0]) / (desviacion[0]*desviacion[0]));
    cout << "Reduccion de varianza: " << reduccionVar << "%" << endl;

    Mat histSuav;
    calcHist(&suavizada, 1, 0, Mat(), histSuav, 1, &histSize, &histRange);
    Mat histImgSuav(histH, histW, CV_8UC1, Scalar(255));
    normalize(histSuav, histSuav, 0, histH, NORM_MINMAX);
    for(int i = 1; i < histSize; i++){
        line(histImgSuav,
             Point(binW*(i-1), histH - cvRound(histSuav.at<float>(i-1))),
             Point(binW*i,     histH - cvRound(histSuav.at<float>(i))),
             Scalar(0), 2);
    }
    imshow("Histograma Suavizada", histImgSuav);
    if(saveImages) imwrite("07_histograma_suavizada.png", histImgSuav);

    Mat laplaciano;
    Laplacian(img, laplaciano, CV_16S, 3);
    convertScaleAbs(laplaciano, laplaciano);
    imshow("Highpass - Laplaciano", laplaciano);
    if(saveImages) imwrite("08_laplaciano.png", laplaciano);

    Mat sharpened;
    add(img, laplaciano, sharpened);
    imshow("Imagen Afilada", sharpened);
    if(saveImages) imwrite("09_imagen_afilada.png", sharpened);

    Scalar mediaLap, desvLap, mediaSharp, desvSharp;
    meanStdDev(laplaciano, mediaLap,   desvLap);
    meanStdDev(sharpened,  mediaSharp, desvSharp);

    cout << "\n--- Filtro de Afilado (Highpass) ---" << endl;
    cout << "Desviacion Laplaciano:     " << desvLap[0]   << endl;
    cout << "Desviacion imagen afilada: " << desvSharp[0] << endl;

    Mat suavPrimero, lapSuav, sharpenedMejor;
    GaussianBlur(img, suavPrimero, Size(5,5), 1.0);
    Laplacian(suavPrimero, lapSuav, CV_16S, 3);
    convertScaleAbs(lapSuav, lapSuav);
    add(suavPrimero, lapSuav, sharpenedMejor);
    imshow("Afilado con suavizado previo", sharpenedMejor);
    if(saveImages) imwrite("10_afilado_suavizado_previo.png", sharpenedMejor);

    Scalar mediaSM, desvSM;
    meanStdDev(sharpenedMejor, mediaSM, desvSM);
    cout << "Desviacion afilado con suavizado previo: " << desvSM[0] << endl;
    cout << "-> Menor desviacion = menos ruido amplificado." << endl;

    cout << "\n--- Pipeline Completo ---" << endl;

    Mat gauss;
    GaussianBlur(img, gauss, Size(7,7), 1.5);

    Mat lap;
    Laplacian(gauss, lap, CV_16S, 3);
    convertScaleAbs(lap, lap);

    Mat final_img;
    add(gauss, lap, final_img);

    imshow("Pipeline Completo", final_img);
    if(saveImages) imwrite("11_pipeline_completo.png", final_img);

    Scalar mediaFinal, desvFinal;
    meanStdDev(final_img, mediaFinal, desvFinal);

    cout << "Desviacion imagen original:  " << desviacion[0] << endl;
    cout << "Desviacion imagen suavizada: " << desv2[0]      << endl;
    cout << "Desviacion pipeline final:   " << desvFinal[0]  << endl;

    Mat histFinal;
    calcHist(&final_img, 1, 0, Mat(), histFinal, 1, &histSize, &histRange);
    Mat histImgFinal(histH, histW, CV_8UC1, Scalar(255));
    normalize(histFinal, histFinal, 0, histH, NORM_MINMAX);
    for(int i = 1; i < histSize; i++){
        line(histImgFinal,
             Point(binW*(i-1), histH - cvRound(histFinal.at<float>(i-1))),
             Point(binW*i,     histH - cvRound(histFinal.at<float>(i))),
             Scalar(0), 2);
    }
    imshow("Histograma Pipeline Final", histImgFinal);
    if(saveImages) imwrite("12_histograma_pipeline_final.png", histImgFinal);

    cout << "\n--- Resumen del Pipeline ---" << endl;
    cout << "Original  -> Suavizado Gaussiano: reduce ruido y speckle." << endl;
    cout << "Suavizado -> Laplaciano:           detecta bordes sin amplificar ruido." << endl;
    cout << "Suma final:                        imagen con bordes realzados y ruido controlado." << endl;

    if(saveImages){
        cout << "\n--- Imágenes Guardadas ---" << endl;
        cout << "Se han guardado 12 imágenes procesadas:" << endl;
        cout << "  01_imagen_original.png" << endl;
        cout << "  02_histograma.png" << endl;
        cout << "  03_segmentacion_2sigma.png" << endl;
        cout << "  04_segmentacion_1sigma.png" << endl;
        cout << "  05_segmentacion_3sigma.png" << endl;
        cout << "  06_suavizada_gaussiana.png" << endl;
        cout << "  07_histograma_suavizada.png" << endl;
        cout << "  08_laplaciano.png" << endl;
        cout << "  09_imagen_afilada.png" << endl;
        cout << "  10_afilado_suavizado_previo.png" << endl;
        cout << "  11_pipeline_completo.png" << endl;
        cout << "  12_histograma_pipeline_final.png" << endl;
    }

    waitKey(0);
    return 0;
}