#include "libretta_interpolator.h"

#include <cmath>
#include <iostream>


CFloatInterpolator::CFloatInterpolator (size_t x_1, float y_1, size_t x_2, float y_2)
{
  y1 = y_1;
  y2 = y_2;
  
  x1 = x_1;
  x2 = x_2;
}


CFloatInterpolatorSimple::CFloatInterpolatorSimple (size_t x_1, float y_1, size_t x_2, float y_2): 
CFloatInterpolator (x_1, y_1, x_2, y_2)
{
  size_t elements_between = x_2 - x_1;
  
  //cout << "y1: " << y1 << endl;
  //cout << "y2: " << y2 << endl;
  
  if (y2 > y1)
     {
      values_diff = y2 - y1;
 
      if (values_diff == 0)
         part = 0.0f;
      else
          part = (float) values_diff / elements_between;

     }
   else  
     {
      float values_diff = y1 - y2;
 
      if (values_diff == 0)
         part = 0;
      else
           part = (float) values_diff / elements_between;
     }     
}


float CFloatInterpolatorSimple::get_y_at_x (size_t x)
{
  if (y2 > y1)
     {
      return (y1 + ((x - x1) * part));
      
     }
  else
      return (y1 - ((x - x1) * part));
     
}

