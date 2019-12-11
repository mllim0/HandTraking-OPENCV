#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <chrono>
 #include "Draw.h"

static auto start = std::chrono::system_clock::now();
 
class HandGesture {

public:

  std::string movimientoMano_;
  float depthError_;
  float motionError_;

  
public:
  HandGesture();
  void FeaturesDetection(cv::Mat mask, cv::Mat output_img);
  std::string motionCapture(const Point& diferencia);
  void mostrarMotion(Mat output_img);

private:
  double getAngle         (cv::Point s, cv::Point e, cv::Point f);
  int    pintarContorno   (Mat output_img, const std::vector<std::vector<Point>>& contours, Mat mask);
  void   pintarConvexHull (Mat output_img, 
                           const std::vector<int>& hull,
                           const std::vector<std::vector<Point>>& contours, 
                           int index);
  Rect getBoundingRect    (const std::vector<std::vector<Point>>& contours, int index);
};
