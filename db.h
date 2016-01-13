#ifndef DB_H
#define DB_H

#include <cmath>

extern double db_scale;

inline float db2lin (float db)
{
  return (float) exp (db * db_scale);
}

/*
static inline float db2lin (float db)
{
  return powf (10.0f, db / 20);
}
*/


inline float float2db (float v)
{
  if (v == 0.0f)
     return 0.0f;
         
  if (v > 0.0f)
     return (float) 20 * log10 (v / 1.0);

  return (float) 20 * log10 (v / -1.0);
}


void init_db();

#endif // DB_H
