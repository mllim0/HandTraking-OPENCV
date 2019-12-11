#include "../include/BoundingRect.h"

BoundingRect& BoundingRect::operator = (const Rect& rectangulo){
  this->Rect::operator=(rectangulo);
  centro_.x = tl().x + rectangulo.width / 2;
  centro_.y = tl().y + rectangulo.height / 2;
  puntoAngular_.x = tl().x + rectangulo.width;
  puntoAngular_.y = centro_.y;
  return *this;
}

BoundingRect& BoundingRect::operator = (const BoundingRect& boundRect)
{
  centro_ = boundRect.centro_;
  puntoAngular_= boundRect.puntoAngular_;
  this->Rect::operator=(boundRect);
  return *this;
}
