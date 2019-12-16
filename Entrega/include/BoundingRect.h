#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace cv;
using namespace std;

/**
 * @brief      This class describes a bounding rectangle.
 */
class BoundingRect : public Rect
{
public:
  Point centro_;
  Point puntoAngular_;

public:
  BoundingRect& operator = (const Rect& rectangulo);
  BoundingRect& operator = (const BoundingRect& boundRect);

};