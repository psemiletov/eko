#ifndef LIBRETTA_INTERPOLATOR_H
#define LIBRETTA_INTERPOLATOR_H

using namespace std;

#include <cstddef>

class CFloatInterpolator
{
public:
 
  float y1;
  float y2;

  size_t x1;
  size_t x2;
  
  CFloatInterpolator (size_t x_1, float y_1, size_t x_2, float y_2);
  virtual ~CFloatInterpolator() {};

  virtual float get_y_at_x (size_t x) = 0;
 
};

class CFloatInterpolatorSimple: public CFloatInterpolator
{
public:
    
  float values_diff; 
  float part;
  
  CFloatInterpolatorSimple (size_t x_1, float y_1, size_t x_2, float y_2);

  float get_y_at_x (size_t x);
};

#endif
