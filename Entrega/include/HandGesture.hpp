#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <set>
#include <memory>

#include "Draw.h"
#include "BoundingRect.h"

#define LOG(X) std::cerr << X << std::endl

#define PORCEN_DEPTH 0.1
#define PORCEN_MOTION 0.05

using namespace cv;
using namespace std;

static auto start = std::chrono::system_clock::now();

/**
 * @brief      This class describes a hand gesture.
 */
class HandGesture 
{

public:

  std::string   movimientoMano_;
  BoundingRect  boundRect_;

  float depthError_;
  float motionError_;

  bool paint_;
  bool changePaintState_;
  Point pPaint_;
  std::set< std::unique_ptr<Point> >sPaint_;

public:

  HandGesture();
  void         FeaturesDetection  (cv::Mat mask, cv::Mat output_img);
  void         mostrarMotion      (Mat output_img);

private:

  Rect    getBoundingRect  (const std::vector<std::vector<Point>>& contours, int index);
  double  getAngle         (cv::Point s, cv::Point e, cv::Point f);
  int     pintarContorno   (Mat output_img, const std::vector<std::vector<Point>>& contours, Mat mask);
  void    pintarConvexHull (Mat output_img, 
                            const std::vector<int>& hull,
                            const std::vector<std::vector<Point>>& contours, 
                            int index);

  void         motionTracking   ();
  std::string  motionCapture    (const Point& diferencia);

  void  paintFunction(Mat output_img);

};
