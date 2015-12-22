#include <iostream>
#include <cmath>


using namespace std;


static inline float
lin2db( float lin )
{
        if (lin == 0.0f) return -90.0f;
        else return (20.0f * log10f(lin));
}


static inline float
db2lin( float db )
{
  return powf(10.0f, db / 20);
}


float conv (float v, float middle, float max)
{
  if (v == middle)
     return 0;
 
  if (v > middle)
    {
     return (max - middle - v);
    }
  else
     {
      //return (middle - v) * -1;
      return middle - v;

     }
}


float conv_to_db (float v, float v_min, float v_max, float range_negative, float range_positive)
{
  if (v == 0)
     return 0;
  
  if (v > 0)
    {
     float x = v_max / range_positive;
     float y = v_max / v;
     
     return v / (y * x);
    }
  else
      {
       float x = v_max / range_negative;
       float y = v_max / v;
     
       return v / (y * x);
      }
}


//out = ((float)in)*(1.0f/32768.0f);

#define VMAX 50
#define VMIN -50


int main (int argc, char *argv[])
{

  cout << "--------" << endl;
 
  float x = conv (60, 50, 100);
  
  cout << "raw: " << x << endl;
  
  float y = conv_to_db (x, VMIN, VMAX, -90.0f, 6.0f);
  
  cout << "db: " << y << endl;
  
  float z = db2lin (y);

  cout << "lin: " << z << endl;
  
  
  return 0;
}
