#ifndef NOISEGEN_H
#define NOISEGEN_H

#include <cstring>


float* noise_generate_white (size_t len, float amp); //gen one channel noise
float* noise_generate_pink (size_t len, float amp); //gen one channel noise
float* noise_generate_pink2 (size_t len, float amp); //gen one channel noise


#endif // NOISEGEN_H
