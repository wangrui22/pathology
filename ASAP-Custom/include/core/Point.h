#ifndef WSCOORDINATE_H
#define WSCOORDINATE_H
#include "core_export.h"

class CORE_EXPORT Point {
public:
  Point();
  Point(const float& x, const float& y);
  inline float getX() const { return _x; }
  inline float getY() const { return _y; };
  inline void  setX(const float& x) { _x = x; }
  inline void  setY(const float& y) { _y = y; }
  inline const Point operator - (const Point& l) const
  {
      Point t;
      t._x = this->_x - l._x;
      t._y = this->_y - l._y;
      return t;
  };

  inline const Point operator + (const Point& l) const
  {
      Point t;
      t._x = this->_x + l._x;
      t._y = this->_y + l._y;
      return t;
  };

private:
	float _x;
	float _y;
};
#endif