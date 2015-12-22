// EQ.C - Main Source file for 3 band EQ
//
// (c) Neil C / Etanza Systems / 2K6
//
// Shouts / Loves / Moans = etanza at lycos dot co dot uk
//
// This work is hereby placed in the public domain for all purposes, including
// use in commercial applications.
//
// The author assumes NO RESPONSIBILITY for any problems caused by the use of
// this software.
//
//----------------------------------------------------------------------------

// NOTES :
//
// - Original filter code by Paul Kellet (musicdsp.pdf)
//
// - Uses 4 first order filters in series, should give 24dB per octave
//
// - Now with P4 Denormal fix :)


//----------------------------------------------------------------------------

// ----------
//| Includes |
// ----------

#include <math.h>
#include <string.h>

#include "3pass_eq.h"


// -----------
//| Constants |
// -----------

static float vsa = (1.0f / 4294967295.0f);   // Very small amount (Denormal Fix)


// ---------------
//| Initialise EQ |
// ---------------

// Recommended frequencies are ...
//
//  lowfreq  = 880  Hz
//  highfreq = 5000 Hz
//
// Set mixfreq to whatever rate your system is using (eg 48Khz)

void init_3band_state (EQSTATE* es, int lowfreq, int highfreq, int mixfreq)
{
  // Clear state

  //memset(es, 0, sizeof(EQSTATE));

  // Set Low/Mid/High gains to unity

  es->lg = 1.0f;
  es->mg = 1.0f;
  es->hg = 1.0f;

  // Calculate filter cutoff frequencies

  es->lf = 2.0f * sin(M_PI * ((float)lowfreq / (float)mixfreq));
  es->hf = 2.0f * sin(M_PI * ((float)highfreq / (float)mixfreq));
}


// ---------------
//| EQ one sample |
// ---------------

// - sample can be any range you like :)
//
// Note that the output will depend on the gain settings for each band
// (especially the bass) so may require clipping before output, but you
// knew that anyway :)

float do_3band (EQSTATE* es, float sample)
{
  // Locals

  float l, m, h;      // Low / Mid / High - Sample Values

  // Filter #1 (lowpass)

  es->f1p0  += (es->lf * (sample - es->f1p0)) + vsa;
  es->f1p1  += (es->lf * (es->f1p0 - es->f1p1));
  es->f1p2  += (es->lf * (es->f1p1 - es->f1p2));
  es->f1p3  += (es->lf * (es->f1p2 - es->f1p3));

  l = es->f1p3;

  // Filter #2 (highpass)

  es->f2p0 += (es->hf * (sample - es->f2p0)) + vsa;
  es->f2p1 += (es->hf * (es->f2p0 - es->f2p1));
  es->f2p2 += (es->hf * (es->f2p1 - es->f2p2));
  es->f2p3 += (es->hf * (es->f2p2 - es->f2p3));

  h = es->sdm3 - es->f2p3;


  // Calculate midrange (signal - (low + high))

  m = sample - (h + l);
  //m          = es->sdm3 - (h + l);

  // Scale, Combine and store

  l *= es->lg;
  m *= es->mg;
  h *= es->hg;

  // Shuffle history buffer

  es->sdm3 = es->sdm2;
  es->sdm2 = es->sdm1;
  es->sdm1 = sample;

  // Return result

  return (l + m + h);
}

