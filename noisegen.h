#ifndef NOISEGEN_H
#define NOISEGEN_H

#include <cstring>


/*from:   Audacity: A Digital Audio Editor

  Noise.cpp

  Dominic Mazzoni
*/

bool MakeNoise (float *buffer, size_t len, float fs, float amplitude, int noiseType);

#endif // NOISEGEN_H
