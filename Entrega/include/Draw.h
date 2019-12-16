#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define w 400
#define ROJO  Scalar(0,0,255)
#define VERDE Scalar(0,255,0)
#define AZUL Scalar(255,0,0)
#define TEXT_ESPESOR 2

using namespace cv;

/**
 * @brief      This class describes a draw.
 */
class Draw
{
public:
  static void filledCircle( Mat img, Point center );
  static void line        ( Mat img, Point start, Point end );

private:
  double distance(const Point& A, const Point& B);
};