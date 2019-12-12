#include "../include/BoundingRect.h"

BoundingRect& BoundingRect::operator = (const Rect& rectangulo){
  this->Rect::operator=(rectangulo);
  centro_.x = (rectangulo.tl().x + rectangulo.width) / 2;
  centro_.y = (rectangulo.tl().y + height) / 2;
  puntoAngular_.x = (rectangulo.tl().x + rectangulo.width) / 2;
  puntoAngular_.y = rectangulo.tl().y;
  return *this;
}

BoundingRect& BoundingRect::operator = (const BoundingRect& boundRect)
{
  centro_ = boundRect.centro_;
  puntoAngular_= boundRect.puntoAngular_;
  this->Rect::operator=(boundRect);
  return *this;
}
