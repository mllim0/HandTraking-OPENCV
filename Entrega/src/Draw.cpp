#include "../include/Draw.h"



/**
 * @brief      Calculate the distance
 *
 * @param[in]  A     { parameter_description }
 * @param[in]  B     { parameter_description }
 *
 * @return     distance
 */
double Draw::distance(const Point& A, const Point& B)
{
  return norm(A-B);
}

/**
 * @brief      Draws a filled circle
 *
 * @param[in]  img     The image
 * @param[in]  center  The center
 */
void Draw::filledCircle(Mat img, Point center)
{
  circle( img,
          center,
          w/32,
          Scalar(0, 0, 255),
          FILLED,
          LINE_8 );
}

/**
 * @brief      Draws a line
 *
 * @param[in]  img    The image
 * @param[in]  start  The start
 * @param[in]  end    The end
 */
void Draw::line(Mat img, Point start, Point end)
{
  cv::line( img,
        start,
        end,
        VERDE
    );
}