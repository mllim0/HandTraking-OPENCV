#include "HandGesture.hpp"

/**
 * @brief      Constructs a new instance.
 */
HandGesture::HandGesture()
{
  paint_ = false;
  changePaintState_ = true;
}

/**
 * @brief      Gets the angle.
 *
 * @param[in]  s     { parameter_description }
 * @param[in]  e     { parameter_description }
 * @param[in]  f     { parameter_description }
 *
 * @return     The angle.
 */
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
  if (angle > CV_PI)  angle -= 2 * CV_PI;
  if (angle < -CV_PI) angle += 2 * CV_PI;
  return (angle * 180.0/CV_PI);
}

/**
 * @brief      Detects the features
 *
 * @param[in]  mask        The mask
 * @param[in]  output_img  The output image
 */
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) 
{
  vector<vector<Point>> contours;
  Mat temp_mask;
  mask.copyTo(temp_mask);
  int index = 0;
  float ladoMedioRec;

  // CODIGO 3.1
  // detección del contorno de la mano y selección del contorno más largo
  circle(temp_mask, Point(10,10), 5, cv::Scalar(255));
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
    double angle = getAngle(s, e, f);
    ladoMedioRec = (boundRect_.height + boundRect_.width)/2;
    depthError_  = ladoMedioRec * PORCEN_DEPTH;
  
    // CODIGO 3.2
    // filtrar y mostrar los defectos de convexidad
    if (angle < 90 && depth > depthError_)
    {
      circle(output_img, f, 5, VERDE, 3);  
      contVerde++;    
    }
    
    if (depth > depthError_){
      circle(output_img, s, 5, ROJO, 3);
      contRojo++;
      if (paint_){
        pPaint_ = s;
      }
    }
  }
  // Error de los pixeles
  motionError_ = ladoMedioRec * PORCEN_MOTION;
  // Capturar movimiento de la mano
  motionTracking();

  if (contVerde >= 1)
  {
    putText(output_img, std::to_string(contVerde+1), Point(10,30), FONT_HERSHEY_PLAIN, 2,  ROJO, TEXT_ESPESOR);
    changePaintState_ = true;
  }
  else 
  {
    putText(output_img, std::to_string(contRojo), Point(10,30), FONT_HERSHEY_PLAIN, 2,  ROJO, TEXT_ESPESOR);
    if(contRojo == 0 && changePaintState_ && !paint_){
      paint_ = true;
      changePaintState_ = false;
    }else if(contRojo == 0 && changePaintState_ && paint_){
      paint_ = false;
      changePaintState_ = false;
      sPaint_.clear();
    }
  }

  if(contRojo == 1 && paint_){
    sPaint_.insert(std::make_unique<Point>(pPaint_));
  }
  if(paint_){
    putText(output_img, "Pintar activado", Point(50,30), FONT_HERSHEY_PLAIN, 2,  ROJO, TEXT_ESPESOR);
    paintFunction(output_img);
  }else{
    putText(output_img, "Pintar desactivado", Point(50,30), FONT_HERSHEY_PLAIN, 2,  ROJO, TEXT_ESPESOR);
  }
  mostrarMotion(output_img);
}

/**
 * @brief      Prints on screen
 *
 * @param[in]  output_img  The output image
 */
void HandGesture::paintFunction(Mat output_img){
  for(std::set<std::unique_ptr<Point> >::iterator it = sPaint_.begin(); it != sPaint_.end(); it++){
    circle(output_img, **it, 3, AZUL, 3);
  }
}

/**
 * @brief      Draws the contorn of the hand
 *
 * @param[in]  output_img  The output image
 * @param[in]  contours    The contours
 * @param[in]  mask        The mask
 *
 * @return     index
 */
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
    drawContours(output_img,
                contours,
                index,
                AZUL, 2, 8,
                vector<Vec4i>(),
                0, Point());
  }

  assert(index != -1);
  return index;
}

/**
 * @brief      Prints the convex hull
 *
 * @param[in]  output_img  The output image
 * @param[in]  hull        The hull
 * @param[in]  contours    The contours
 * @param[in]  index       The index
 */
void HandGesture::pintarConvexHull (Mat output_img, 
                                    const std::vector<int>& hull, 
                                    const std::vector<std::vector<Point>>& contours,
                                    int index)
{
  Point pt0 = contours[index][hull[hull.size()-1]];
  for (size_t i = 0; i < hull.size(); i++)
  {
    Point pt = contours[index][hull[i]];
    line(output_img, pt0, pt, ROJO, 2, CV_AA);
    pt0 = pt;
  }
}

/**
 * @brief      Gets the bounding rectangle.
 *
 * @param[in]  contours  The contours
 * @param[in]  index     The index
 *
 * @return     The bounding rectangle.
 */
Rect HandGesture::getBoundingRect (const std::vector<std::vector<Point>>& contours, int index)
{
  Rect boundRect;
  vector<Point> counterPoly;

  // Generamos el bounding rect para el contorno de la mano.
  approxPolyDP( Mat(contours[index]), counterPoly, 3, true );
  boundRect = boundingRect( Mat(counterPoly));

  return boundRect;
}

/**
 * @brief      { function_description }
 *
 * @param[in]  diferencia  The diferencia
 *
 * @return     { description_of_the_return_value }
 */
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

/**
 * @brief      Prints text on screen
 *
 * @param[in]  output_img  The output image
 */
void HandGesture::mostrarMotion(Mat output_img)
{
  putText(output_img, movimientoMano_, Point(80,80), FONT_HERSHEY_PLAIN, 2,  ROJO, TEXT_ESPESOR);
}

/**
 * @brief      Detects the moves
 */
void HandGesture::motionTracking()
{
  static bool isFirstFrame = false;
  static Point centroMasaManoIni;
  // Comprobamos que ha pasado el tiempo asignado para hacer un tracking del movimiento realizado
  auto actualTime = std::chrono::system_clock::now();
  std::chrono::duration<float> diferenciaTiempo = actualTime - start;
  if(isFirstFrame)
  {
    isFirstFrame  = false;
    centroMasaManoIni = boundRect_.centro_;
  }
  else if (diferenciaTiempo.count() >= 0.3f)
  {
    isFirstFrame = true;
    start = std::chrono::system_clock::now();
    Point diferencia = boundRect_.centro_ - centroMasaManoIni;
    motionCapture(diferencia);
  }
}