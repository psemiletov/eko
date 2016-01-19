/*
  GPL'ed code from Audacity
  Noise.cpp
  by Dominic Mazzoni
*/

#include <cmath>
#include <cstdlib>

#include <random>

#include "noisegen.h"


float* noise_generate_white (size_t len, float amp) //gen one channel noise
{
  float *buffer = new float [len];
  
  std::random_device rd;
  std::default_random_engine gen (rd());
  std::uniform_real_distribution<> dis(-1.0, 1.0);
  
 for (size_t i = 0; i < len; i++)
      {
       buffer[i] = (float)dis(gen) * amp;  
      }
          
  return buffer;        
}

/*
Following the
http://www.firstpr.com.au/dsp/pink-noise/
Using the Paul Kellet's filtering
*/

float* noise_generate_pink (size_t len, float amp) //gen one channel noise
{
  float *buffer = new float [len];
  
  float b0, b1, b2;
  
  std::random_device rd;
  std::knuth_b gen (rd());
  std::uniform_real_distribution<> dis(-1.0, 1.0);
  
  for (size_t i = 0; i < len; i++)
      {
       float white = (float)dis(gen);  
       
       b0 = 0.99765 * b0 + white * 0.0990460;
       b1 = 0.96300 * b1 + white * 0.2965164;
       b2 = 0.57000 * b2 + white * 1.0526913;
       float tmp = (b0 + b1 + b2 + white * 0.1848) / 4; 
      
       buffer[i] = tmp * amp;
      }
          
  return buffer;        
}

//Using the Paul Kellet's filtering
float* noise_generate_pink2 (size_t len, float amp) //gen one channel noise
{
  float *buffer = new float [len];
  
  float b0, b1, b2, b3, b4, b5, b6;
  b6 = 0;
   
  std::random_device rd;
  std::knuth_b gen (rd());
  std::uniform_real_distribution<> dis(-1.0, 1.0);
  
  for (size_t i = 0; i < len; i++)
      {
       float white = (float)dis(gen);  
       
       b0 = 0.99886 * b0 + white * 0.0555179;
       b1 = 0.99332 * b1 + white * 0.0750759;
       b2 = 0.96900 * b2 + white * 0.1538520;
       b3 = 0.86650 * b3 + white * 0.3104856;
       b4 = 0.55000 * b4 + white * 0.5329522;
       b5 = -0.7616 * b5 - white * 0.0168980;
       float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362;
       b6 = white * 0.115926;
     
       buffer[i] = pink * amp / 7;
      }
          
  return buffer;        
}


//brown B[n] = (B[n-1] + C * w[n]) / (1. + C) 
