#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define w 400
using namespace cv;

class Draw
{
public:
  static void filledCircle( Mat img, Point center );
  static void line        ( Mat img, Point start, Point end );

private:

  double distance(const Point& A, const Point& B);
};