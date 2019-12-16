#include "MyBGSubtractorColor.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;



/**
 * @brief      Constructs a new instance.
 *
 * @param[in]  vc    { parameter_description }
 */
MyBGSubtractorColor::MyBGSubtractorColor(VideoCapture vc) { 

  cap = vc;
  max_samples = MAX_HORIZ_SAMPLES * MAX_VERT_SAMPLES; //Cuadrados verdes Zonas de inspección
  
  lower_bounds = vector<Scalar>(max_samples); //Vector para las muestras
  upper_bounds = vector<Scalar>(max_samples);
  means = vector<Scalar>(max_samples); //Vector para almacenar las medias
  
  h_low = 12;
    h_up = 7;
  l_low = 30;
  l_up = 40;
  s_low = 80;
  s_up = 80;

  
  namedWindow("Trackbars");

  createTrackbar("H low:", "Trackbars", &h_low, 100, &MyBGSubtractorColor::Trackbar_func);
  createTrackbar("H high:", "Trackbars", &h_up, 100, &MyBGSubtractorColor::Trackbar_func);
  createTrackbar("L low:", "Trackbars", &l_low, 100, &MyBGSubtractorColor::Trackbar_func);
  createTrackbar("L high:", "Trackbars", &l_up, 100, &MyBGSubtractorColor::Trackbar_func);
  createTrackbar("S low:", "Trackbars", &s_low, 100, &MyBGSubtractorColor::Trackbar_func);
  createTrackbar("S high:", "Trackbars", &s_up, 100, &MyBGSubtractorColor::Trackbar_func);


}

/**
 * @brief      { function_description }
 *
 * @param[in]  <unnamed>  { parameter_description }
 * @param      <unnamed>  { parameter_description }
 */
void MyBGSubtractorColor::Trackbar_func(int, void*)
{

}


/**
 * @brief      Detects he model of the hand
 */
void MyBGSubtractorColor::LearnModel() { 
  //Práctica 1
  Mat frame, tmp_frame, hls_frame; //hls es la img de salida
  std::vector<cv::Point> samples_positions;

  cap >> frame;
  
//Convertir imagen a hls

  //almacenamos las posiciones de las esquinas de los cuadrados 
  Point p;
  for (int i = 0; i < MAX_HORIZ_SAMPLES; i++) {
    for (int j = 0; j < MAX_VERT_SAMPLES; j++) {
      p.x = frame.cols / 2 + (-MAX_HORIZ_SAMPLES / 2 + i)*(SAMPLE_SIZE + DISTANCE_BETWEEN_SAMPLES);
      p.y = frame.rows / 2 + (-MAX_VERT_SAMPLES / 2 + j)*(SAMPLE_SIZE + DISTANCE_BETWEEN_SAMPLES);
      samples_positions.push_back(p); 
    }
  }

  namedWindow("Cubre los cuadrados con la mano y pulsa espacio");

  for (;;) {
    
    flip(frame, frame, 1);
    
    frame.copyTo(tmp_frame); //Copia para evitar errores al hacer la media con frame

    //dibujar los cuadrados
    
    for (int i = 0; i < max_samples; i++) {
      rectangle(tmp_frame, Rect(samples_positions[i].x, samples_positions[i].y,
              SAMPLE_SIZE, SAMPLE_SIZE), Scalar(0, 255, 0), 2); //Creación de los cuadrados verdes sobre la imagen
    }
    
  

    imshow("Cubre los cuadrados con la mano y pulsa espacio", tmp_frame);
    char c = cvWaitKey(40);
    if (c == ' ')
    {
      break;
    }
    cap >> frame;
  }

        // CODIGO 1.1
        // Obtener las regiones de interés y calcular la media de cada una de ellas
        // almacenar las medias en la variable means
        // ...

        cvtColor(frame, hls_frame, CV_BGR2HLS);

        for (int i = 0; i < max_samples; i++)
        {
          Mat imagen = hls_frame(Rect(samples_positions[i].x, samples_positions[i].y, SAMPLE_SIZE, SAMPLE_SIZE));
          cv::Scalar media = mean(imagen);
          means[i] = media;
        }

        destroyWindow("Cubre los cuadrados con la mano y pulsa espacio");

}
void  MyBGSubtractorColor::ObtainBGMask(cv::Mat frame, cv::Mat &bgmask) {
        
        // CODIGO 1.2
        // Definir los rangos máximos y mínimos para cada canal (HLS) 
        // umbralizar las imágenes para cada rango y sumarlas para
        // obtener la máscara final con el fondo eliminado
        //...

    Mat hls_frame;
    vector<cv::Mat> imagenes(18);
    cvtColor(frame, hls_frame, CV_BGR2HLS);
    size_t j = 0;
    int a,b,c,d,e,f;
    //cv::Scalar h_l(h_low), h_u(h_up), s_l(s_low), s_u(s_up), l_l(l_low), l_u(l_up);

    for (auto i = 0; i < 18; i++)
    {
      
      if ((means[i][0] - h_low) > 0) 
        a = means[i][0] - h_low;
      else
        a = 0;

      if ((means[i][0] + h_up) < 255) 
        b = means[i][0] + h_up;
      else
        b = 255;

      if ((means[i][2] - l_low) > 0) 
        c = means[i][2] - l_low;
      else
        c = 0;

      if ((means[i][2] + l_up) < 255) 
        d = means[i][2] + l_up;
      else
        d = 255;

      if ((means[i][1] - s_low) > 0) 
        e = means[i][1] - s_low;
      else
        e = 0;

      if ((means[i][1] + s_up) < 255) 
        f = means[i][1] + s_up;
      else
        f = 255;

      cv::Scalar low(a, e, c);
      cv::Scalar up(b,f,d);
      inRange(hls_frame, low, up, imagenes[j++]);
    }

    Mat background(hls_frame.rows, hls_frame.cols, CV_8U);
    background.setTo(cv::Scalar(0));

    for (j = 0; j < imagenes.size(); j++)
      background = background + imagenes[j];

    background.copyTo(bgmask);
}
