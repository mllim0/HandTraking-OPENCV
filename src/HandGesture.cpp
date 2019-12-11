#include "HandGesture.hpp"

HandGesture::HandGesture()
{}


double HandGesture::getAngle(Point s, Point e, Point f)
{
  
  double v1[2],v2[2];
  v1[0] = s.x - f.x;
  v1[1] = s.y - f.y;

  v2[0] = e.x - f.x;
  v2[1] = e.y - f.y;

  double ang1 = atan2(v1[1],v1[0]);
  double ang2 = atan2(v2[1],v2[0]);

  double angle = ang1 - ang2;
  if (angle > CV_PI) angle -= 2 * CV_PI;
  if (angle < -CV_PI) angle += 2 * CV_PI;
  return (angle * 180.0/CV_PI);
}

void HandGesture::FeaturesDetection(Mat mask, Mat output_img) 
{

  vector<vector<Point>> contours;
  Mat temp_mask;
  mask.copyTo(temp_mask);
  int index = 0;
  float ladoMedioRec;

  // CODIGO 3.1
  // detección del contorno de la mano y selección del contorno más largo

  Point centro(10,10);
  circle(temp_mask, centro, 5, cv::Scalar(255));
  findContours(temp_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  index = pintarContorno(output_img, contours, temp_mask);

  //obtener el convex hull  
  vector<int> hull;
  convexHull(contours[index],hull);
  pintarConvexHull(output_img, hull, contours, index);
  
  //obtener los defectos de convexidad
  vector<Vec4i> defects;
  convexityDefects(contours[index], hull, defects);

  //  Bounding rect
  boundRect_ = getBoundingRect(contours, index); 
  // Mostramos el boundingRect 
  rectangle(output_img, boundRect_.tl(), boundRect_.br(), Scalar(155,155,0));
    
  int contRojo = 0, contVerde = 0;
  for (size_t i = 0; i < defects.size(); i++)
  {
    Point s = contours[index][defects[i][0]];
    Point e = contours[index][defects[i][1]];
    Point f = contours[index][defects[i][2]];

    float depth  = (float)defects[i][3] / 256.0;
    ladoMedioRec = (boundRect_.height + boundRect_.width)/2;
    double angle = getAngle(s, e, f);
    depthError_  = ladoMedioRec * PORCEN_DEPTH;
  
    // CODIGO 3.2
    // filtrar y mostrar los defectos de convexidad
    if (angle < 90 && depth > depthError_)
    {
      circle(output_img, f, 5, Scalar(0,255,0), 3);  
      contVerde++;    
    }
    
    if (depth > depthError_){
      circle(output_img, s, 5, Scalar(0,0,255), 3);
      contRojo++;
    }
  }
  // Error de los pixeles
  motionError_ = ladoMedioRec * PORCEN_MOTION;
  // Capturar movimiento de la mano
  motionTracking();

  if (contVerde >= 1)
  {
    putText(output_img,std::to_string(contVerde+1), Point(10,30), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
  }
  else 
  {
    putText(output_img,std::to_string(contRojo), Point(10,30), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
  }


  
  mostrarMotion(output_img);
}

int HandGesture::pintarContorno(Mat output_img, const std::vector<std::vector<Point>>& contours, Mat mask)
{
  int index = -1;

  if (!mask.empty())
  {
    index   = 0;
    int aux = contours[0].size();
  
    for (size_t i = 1; i < contours.size();i++)
    {
      if (contours[i].size() > (size_t)aux)
      {
        index = i;
        aux   = contours[i].size();
      }
    }
    drawContours(output_img, contours, index, cv::Scalar(255,0,0), 2, 8, vector<Vec4i>(), 0, Point());
  }

  assert(index != -1);
  return index;
}

void HandGesture::pintarConvexHull (Mat output_img, 
                                    const std::vector<int>& hull, 
                                    const std::vector<std::vector<Point>>& contours,
                                    int index)
{
  Point pt0 = contours[index][hull[hull.size()-1]];
  for (size_t i = 0; i < hull.size(); i++)
  {
    Point pt = contours[index][hull[i]];
    line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
    pt0 = pt;
  }
}

Rect HandGesture::getBoundingRect (const std::vector<std::vector<Point>>& contours, int index)
{
  Rect boundRect_;
  vector<Point> counterPoly;

  // Generamos el bounding rect para el contorno de la mano.
  approxPolyDP( Mat(contours[index]), counterPoly, 3, true );
  boundRect_ = boundingRect( Mat(counterPoly));

  return boundRect_;
}

std::string HandGesture::motionCapture(const Point& diferencia)
{
  bool isParado = false;
  std::string movimiento;

  if(diferencia.x > motionError_)
  {
    movimiento += "Izquierda";  
  }
  else if (diferencia.x < -motionError_)
  {
    movimiento += "Derecha";
  }
  else
  {
    isParado = true;
  }
  

  if(diferencia.y > motionError_)
  {
    movimiento += " Abajo";  
  }
  else if (diferencia.y < -motionError_)
  {
    movimiento += " Arriba";
  }
  else
  {
    if (isParado) movimiento = "Parado";
  }
  
  // Aqui eje y
  movimientoMano_ = movimiento;
  return movimientoMano_;
}

void HandGesture::mostrarMotion(Mat output_img)
{
  putText(output_img, movimientoMano_, Point(80,80), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
}

void HandGesture::motionTracking()
{
  static bool isFirstFrame = false;
  static Point centroMasaManoIni;
  // Comprobamos que ha pasado el tiempo asignado para hacer un tracking del movimientorealizado
  auto actualTime = std::chrono::system_clock::now();
  std::chrono::duration<float> diferenciaTiempo = actualTime - start;
  if(isFirstFrame)
  {
    isFirstFrame  = false;
    Point topLeft = boundRect_.tl();
    size_t x = (topLeft.x + boundRect_.width) / 2;
    size_t y = (topLeft.y + boundRect_.height) / 2;
    centroMasaManoIni = Point(x,y);
  }
  else if (diferenciaTiempo.count() >= 0.3f)
  {
    isFirstFrame = true;
    start = std::chrono::system_clock::now();
    
    Point topLeft = boundRect_.tl();
    size_t x = (topLeft.x + boundRect_.width) / 2;
    size_t y = (topLeft.y + boundRect_.height) / 2;
    Point currentPoint(x,y);
    Point diferencia = currentPoint - centroMasaManoIni;
    motionCapture(diferencia);
  }
}