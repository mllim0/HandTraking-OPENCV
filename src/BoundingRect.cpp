#include "../include/BoundingRect.h"

BoundingRect& BoundingRect::operator = (const Rect& rectangulo){
  this->Rect::operator=(rectangulo);
  centro_.x = rectangulo.width / 2;
  centro_.y = rectangulo.height / 2;
  puntoAngular_.x = rectangulo.width;
  puntoAngular_.y = rectangulo.height / 2;
  return *this;
}

BoundingRect& BoundingRect::operator = (const BoundingRect& boundRect)
{
  centro_ = boundRect.centro_;
  puntoAngular_= boundRect.puntoAngular_;
  this->Rect::operator=(boundRect);
  return *this;
}
