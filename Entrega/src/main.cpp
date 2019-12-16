#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
 
#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;

int main()
{

  Mat frame, bgmask, out_frame; //frame -> webcam / bgmask -> img binaria resultante / sustracción out-frame -> La salida
  
  //Abrimos la webcam
  VideoCapture cap;
  cap.open(0);

  if (!cap.isOpened())
  {
    printf("\nNo se puede abrir la c�mara\n");
    return -1;
  }

  int cont = 0;
  while (frame.empty()&& cont < 2000 ) 
  {
     //Control de que la cámara funciona
    cap >> frame;
    ++cont;
  }

  if (cont >= 2000) 
  {
    printf("No se ha podido leer un frame v�lido\n");
    exit(-1);
  }

  // Creamos las ventanas que vamos a usar en la aplicaci�n
  namedWindow("Reconocimiento");
  namedWindow("Fondo");


  // creamos el objeto para la substracci�n de fondo
  MyBGSubtractorColor color_substractor(cap);
  color_substractor.LearnModel();
  
  // creamos el objeto para el reconocimiento de gestos
  HandGesture mano;
  // iniciamos el proceso de obtenci�n del modelo del fondo
  
  start = std::chrono::system_clock::now();
  for (;;)
  {
    cap >> frame; //Captura de frames
    //flip(frame, frame, 1);
    if (frame.empty())
    {
      printf("Le�do frame vac�o\n");
      continue;
    }
    int c = cvWaitKey(40);
    if ((char)c == 'q') break;
    
    color_substractor.ObtainBGMask(frame,bgmask);
    
    
    // obtenemos la m�scara del fondo con el frame actual
                
    // CODIGO 2.1
    // limpiar la m�scara del fondo de ruido
    //...
    threshold(bgmask,bgmask, 1.f, 255, 0);
    //medianBlur(bgmask, bgmask, 3);
    //threshold(bgmask,bgmask, 1.f, 255, 3);
    
    int dilatation_size = 2;
    Mat element = getStructuringElement(MORPH_RECT, Size(2* dilatation_size + 1 , 2* dilatation_size +1), Point(dilatation_size, dilatation_size));
    //erode(bgmask, bgmask, element);
    // dilatation_size = 1;
    // element = getStructuringElement(MORPH_RECT, Size(2* dilatation_size + 1 , 2* dilatation_size +1), Point(dilatation_size, dilatation_size));
     dilate(bgmask, bgmask, element);

dilatation_size = 2;
    element = getStructuringElement(MORPH_RECT, Size(2* dilatation_size + 1 , 2* dilatation_size +1), Point(dilatation_size, dilatation_size));
    erode(bgmask, bgmask, element);


    // deteccion de las caracter�sticas de la mano
    mano.FeaturesDetection(bgmask,frame);
    // mostramos el resultado de la sobstracci�n de fondo
    imshow("Reconocimiento", frame); // Sin tocar
    imshow("Máscara", bgmask);  // Binario

    // mostramos el resultado del reconocimento de gestos
  }
  
  destroyWindow("Reconocimiento");
  destroyWindow("Fondo");
  cap.release();
  return 0;
}
