#ifndef FXFILTER_H
#define FXFILTER_H

/*

this code based on Martin Finke's filter tutorial,
where the filter itself based on the resonant filter 
by Paul Kellett http://www.musicdsp.org/showone.php?id=29
*/

#include <cstring>
#include <cmath>

enum filter_mode {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS
      };

class CFilter 
{
public:

  float resonance;
  int mode;
  float feedback_amount;
  float cutoff;

  float bufl0;
  float bufl1;
  float bufl2;
  float bufl3;
  float bufr0;
  float bufr1;
  float bufr2;
  float bufr3;
    
   CFilter();
    
   void reset(); 
   float process (float sample, size_t channel);
   inline void set_cutoff (float v) {cutoff = v;  calc_feedback_amount();};
   inline void set_resonance (float v) {resonance = v;  calc_feedback_amount();};
   inline void calc_feedback_amount() {feedback_amount = resonance + resonance / (1.0 - cutoff);}; 

};



#endif // FXFILTER_H
