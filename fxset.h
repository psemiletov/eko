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
#include <QDial>

#include <cmath>

#include "floatbuffer.h"
#include "fx-filter.h"
#include "afx.h"



class CFxSimpleAmp: public AFx
{
  Q_OBJECT

public:

  float gain;

  CFxSimpleAmp();
  ~CFxSimpleAmp();

  QLabel *label;

  AFx* self_create();

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

  CFxSimpleOverdrive();
  ~CFxSimpleOverdrive();

  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);

public slots:

  void dial_gain_valueChanged (int value);
  void dial_level_valueChanged (int value);
};


class CFxDelay: public AFx
{
  Q_OBJECT

public:

  QLabel *label;
  
  CFloatBuffer *fb;  
  
  
  QDoubleSpinBox *spb_mixlevel;
  QDoubleSpinBox *spb_time; 
 
  float mixlevel;
  float delay_msecs;

  CFxDelay();
  ~CFxDelay();

  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);


public slots:

  void spb_mixlevel_changed (double value);
  void spb_time_changed (double value);

};


class CFxSimpleFilter: public AFx
{
  Q_OBJECT

public:

  QDoubleSpinBox *dsb_cutoff_freq;
  QDoubleSpinBox *dsb_reso;
  QComboBox *cmb_filter_mode;


  CFilter filter;

  CFxSimpleFilter();

  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

 void cmb_filter_mode_currentIndexChanged (int index);
 void dsb_cutoff_valueChanged (double d);
 void dsb_reso_valueChanged (double d);
};


class CMetaluga: public AFx
{
  Q_OBJECT

public:

  CFilter filter;
  
  float gain;
  float drive;
  float tone;
  float level;
  
  QDial *dial_tone;

  CMetaluga();
  ~CMetaluga();

  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

  void dial_gain_valueChanged (int value);
  void dial_drive_valueChanged (int value);
  void dial_tone_valueChanged (int value);
  void dial_level_valueChanged (int value);
};


#endif // FX_H
