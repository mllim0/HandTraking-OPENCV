#include "../include/Draw.h"



double Draw::distance(const Point& A, const Point& B)
{
  return norm(A-B);
}

void Draw::filledCircle(Mat img, Point center, int tam)
{
  circle( img,
          center,
          tam,
          Scalar(0, 0, 255),
          FILLED,
          LINE_8 );
}

void Draw::line(Mat img, Point start, Point end)
{
  cv::line( img,
        start,
        end,
        VERDE
    );
}