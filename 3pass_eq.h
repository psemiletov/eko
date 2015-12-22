#ifndef __EQ3BAND__
#define __EQ3BAND__

//---------------------------------------------------------------------------
//
//                                3 Band EQ :)
//
// EQ.H - Header file for 3 band EQ
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

//modified bt semiletov

// ------------
//| Structures |
// ------------

typedef struct
{
  // Filter #1 (Low band)

  float lf;       // Frequency
  float f1p0;     // Poles ...
  float f1p1;
  float f1p2;
  float f1p3;

  // Filter #2 (High band)

  float hf;       // Frequency
  float f2p0;     // Poles ...
  float f2p1;
  float f2p2;
  float f2p3;

  // Sample history buffer

  float sdm1;     // Sample data minus 1
  float sdm2;     //                   2
  float sdm3;     //                   3

  // Gain Controls

  float lg;       // low  gain
  float mg;       // mid  gain
  float hg;       // high gain

} EQSTATE;


// ---------
//| Exports |
// ---------

/*extern */void init_3band_state(EQSTATE* es, int lowfreq, int highfreq, int mixfreq);
/*extern */float do_3band(EQSTATE* es, float sample);


#endif // #ifndef __EQ3BAND__
