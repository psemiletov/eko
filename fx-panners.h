#ifndef FXPANNERS_H
#define FXPANNERS_H


//linear panner, law: -6 dB
inline void pan_linear6 (float &l, float& r, float p)
{
  l = 1 - p;
  r = p;
}


//linear panner, law: 0 dB
inline void pan_linear0 (float &l, float& r, float p)
{
  l = 0.5 + (1 - p);
  r = 0.5 + p;
}


//square root panner, law: -3 dB
inline void pan_sqrt (float &l, float& r, float p)
{
  l = sqrt (1 - p); 
  r = sqrt (p);
}


//sin/cos panner, law: -3 dB
inline void pan_sincos (float &l, float& r, float p)
{
  float pan = 0.5 * M_PI * p;
  l = cos (pan);
  r = sin (pan);
}

inline void pan_sincos_v2 (float &l, float& r, float p)
{
  float pan = p * M_PI / 2;
  l = l * sin (pan);
  r = r * cos (pan);
}


#endif // FXPANNERS_H
