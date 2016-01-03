#ifndef FX_H
#define FX_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QTreeView>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QVBoxLayout>

#include <cmath>

#include "floatbuffer.h"
#include "fx-filter.h"
#include "afx.h"



class CRingbuffer
{
public:

   float *buffer;
   size_t samples_max; //buffer size in samples
   size_t current_size; //how manu samples are in the buffer?
   size_t head; //index
   size_t tail; //index
   size_t ret_value;
   
   CRingbuffer (size_t samples, float def_value = 0);
   void prepare_buffer (size_t samples);
   ~CRingbuffer();
   
   void put (float f);
   float get();
};


/*
class CFxSimpleEQ: public AFx
{
 Q_OBJECT

public:

  EQSTATE eq_state_ch00;
  EQSTATE eq_state_ch01;

  CFxSimpleEQ (size_t srate);
  ~CFxSimpleEQ();

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);

public slots:

  void dial_low_valueChanged (int value);
  void dial_mid_valueChanged (int value);
  void dial_hi_valueChanged (int value);

 // void load_from_string (const QString &s) {};
  //QString save_to_string()  {};

};
*/

class CFxSimpleAmp: public AFx
{
  Q_OBJECT

public:

  float gain;

  CFxSimpleAmp (size_t srate);
  ~CFxSimpleAmp();

  QLabel *label;

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);

public slots:

  void dial_gain_valueChanged (int value);
};


class CFxSimpleOverdrive: public AFx
{
  Q_OBJECT

public:

  float gain;
  float level;

  CFxSimpleOverdrive (size_t srate);
  ~CFxSimpleOverdrive();

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);

public slots:

  void dial_gain_valueChanged (int value);
  void dial_level_valueChanged (int value);
};


class CFxPitchShift: public AFx
{
  Q_OBJECT

public:

  QLabel *label;
  
  CFloatBuffer *fb;  
  
  double ratio;

  CFxPitchShift (size_t srate);
  ~CFxPitchShift();

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);


public slots:

  void spb_ratio_changed (double value);
};


class CFxSimpleFilter: public AFx
{
  Q_OBJECT

public:

  QDoubleSpinBox *dsb_cutoff_freq;
  QDoubleSpinBox *dsb_reso;
  QComboBox *cmb_filter_mode;


  CFilter filter;

  CFxSimpleFilter (size_t srate);
 // ~CFxSimpleAmp();


  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

 void cmb_filter_mode_currentIndexChanged (int index);
 void dsb_cutoff_valueChanged (double d);
 void dsb_reso_valueChanged (double d);
 
//  void dial_gain_valueChanged (int value);
};




class CFxClassicOverdrive: public AFx
{
  Q_OBJECT

public:

  CFilter filter;
  
  float gain;
  float drive;
  float tone;
  float level;

  CFxClassicOverdrive (size_t srate);
  ~CFxClassicOverdrive();

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

  void dial_gain_valueChanged (int value);
  void dial_drive_valueChanged (int value);
  void dial_tone_valueChanged (int value);
  void dial_level_valueChanged (int value);
};


#endif // FX_H
