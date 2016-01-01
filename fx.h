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


enum FxState {
              FXS_STOP,
              FXS_RUN,
              FXS_PAUSE
              };



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


class AFx: public QObject
{
  Q_OBJECT

public:
  
  FxState state;

  bool bypass;
  bool realtime;

  bool ui_visible;
  
  CFloatBuffer *float_buffer;  //inner buffer for some purposes
  
  size_t channels;
  size_t samplerate;
  
  QWidget *wnd_ui;
  QVBoxLayout *vbl_main;
  
  QWidget *w_caption;
  QLabel *l_caption;
  QLabel *l_subcaption;
  
  QString name;

  AFx (size_t srate);
  virtual ~AFx();

  virtual size_t execute (float **input, float **output, size_t frames) = 0;
  virtual void set_state (FxState s);
  virtual void reset_params (size_t srate, size_t ch);

  void set_caption (const QString &capt, const QString &subcapt);

  void show_ui();

  //virtual void set_parameter (const QString &param, const QString &value) = 0;

  virtual AFx* self_create (size_t srate) = 0;
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


  CIIRFilter filter;

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


//available fx
class CFxList: public QObject
{
public:

 QList < AFx *> list;

 CFxList();
 ~CFxList();
 
 AFx *find_by_name (const QString &fxname);
 QStringList names();
};



class CFxClassicOverdrive: public AFx
{
  Q_OBJECT

public:

  CIIRFilter filter;
  
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
