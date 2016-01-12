/*
  GPL'ed code from Audacity
  Noise.cpp
  by Dominic Mazzoni
*/

#include <cmath>
#include <cstdlib>

#include "noisegen.h"

bool MakeNoise (float *buffer, size_t len, float fs, float amplitude, int noiseType)
{
   float white, buf0, buf1, buf2, buf3, buf4, buf5;
   float a0, b1, fc, y;
   size_t i;
   float div = ((float)RAND_MAX) / 2.0f;

   switch (noiseType) {
   default:
   case 0: // white
       for(i=0; i<len; i++)
          buffer[i] = amplitude * ((rand() / div) - 1.0f);
       break;

   case 1: // pink
       white=buf0=buf1=buf2=buf3=buf4=buf5=0;

       // 0.55f is an experimental normalization factor: thanks to Martyn
       amplitude *= 0.55f;
       for(i=0; i<len; i++) {
        white=(rand() / div) - 1.0f;
        buf0=0.997f * buf0 + 0.029591f * white;
        buf1=0.985f * buf1 + 0.032534f * white;
        buf2=0.950f * buf2 + 0.048056f * white;
        buf3=0.850f * buf3 + 0.090579f * white;
        buf4=0.620f * buf4 + 0.108990f * white;
        buf5=0.250f * buf5 + 0.255784f * white;
        buffer[i] = amplitude * (buf0 + buf1 + buf2 + buf3 + buf4 + buf5);
       };
       break;

   case 2: // brown
       // fc=100 Hz,
       // y[n]=a0*x[n] + b1*y[n-1];
       white = a0 = b1 = fc = y = 0;
       fc=100; //fs=44100;
       b1=exp(-2*M_PI*fc/fs);
       a0=1.0f-b1;

       // 6.2f is an experimental normalization factor: thanks to Martyn
       amplitude *= 6.2f;
       for(i=0; i<len; i++){
         white=(rand() / div) - 1.0f;
         y = (a0 * white + b1 * y);
         buffer[i] = amplitude * y;
       };
       break;
   }
   return true;
}


