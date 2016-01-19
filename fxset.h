#ifndef FXSET_H
#define FXSET_H

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
  QDial *dial_gain;

  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);

  QString save_params_to_string();
  void load_params_from_string (const QString &s);


public slots:

  void dial_gain_valueChanged (int value);
};


class CFxSimpleOverdrive: public AFx
{
  Q_OBJECT

public:

  float gain;
  float level;

  QDial *dial_gain;
  QDial *dial_level;

  CFxSimpleOverdrive();
  ~CFxSimpleOverdrive();

  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);

  QString save_params_to_string();
  void load_params_from_string (const QString &s);
  
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

  QString save_params_to_string() {return QString ("");};
  void load_params_from_string (const QString &s) {};

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

   QString save_params_to_string() {return QString ("");};
  void load_params_from_string (const QString &s) {};
  
  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

 void cmb_filter_mode_currentIndexChanged (int index);
 void dsb_cutoff_valueChanged (double d);
 void dsb_reso_valueChanged (double d);
};


class CFxMetaluga: public AFx
{
  Q_OBJECT

public:

  CFilter filter;
  
  float gain;
  float drive;
  float tone;
  float level;
  
  QDial *dial_tone;

  CFxMetaluga();
  ~CFxMetaluga();

  AFx* self_create();

 QString save_params_to_string() {return QString ("");};
 void load_params_from_string (const QString &s) {};

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

  void dial_gain_valueChanged (int value);
  void dial_drive_valueChanged (int value);
  void dial_tone_valueChanged (int value);
  void dial_level_valueChanged (int value);
};


class CFxJest: public AFx
{
  Q_OBJECT

public:

  CFilter filter;
  
  float gain;
  float drive;
  float tone;
  float level;
  
  QDial *dial_tone;

  CFxJest();
  //~CFxMetaluga();


 QString save_params_to_string() {return QString ("");};
  void load_params_from_string (const QString &s) {};
  
  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

  void dial_gain_valueChanged (int value);
  void dial_drive_valueChanged (int value);
  void dial_tone_valueChanged (int value);
  void dial_level_valueChanged (int value);
};



class CFxVynil: public AFx
{
  Q_OBJECT

public:

  
  QDial *dial_scratches_amount;
  QDoubleSpinBox *spb_mixlevel;

  float mixlevel;
  float cutoff;

  CFxVynil();

  QString save_params_to_string() {return QString ("");};
  void load_params_from_string (const QString &s) {};
  
  AFx* self_create();

  size_t execute (float **input, float **output, size_t frames);
  void reset_params (size_t srate, size_t ch);

public slots:

  void dial_scratches_amount_valueChanged (int value);
  void spb_mixlevel_changed (double value);
};


#endif // FXSET_H
