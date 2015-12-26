#ifndef FX_H
#define FX_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QTreeView>

#include <cmath>

#include "floatbuffer.h"
#include "3pass_eq.h"

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
  
  QString name;

  AFx (size_t srate);
  virtual ~AFx();

  virtual size_t execute (float **input, float **output, size_t frames) = 0;
  virtual void set_state (FxState s);
  virtual void reset_params (size_t srate, size_t channels);

  void show_ui();

  //virtual void set_parameter (const QString &param, const QString &value) = 0;

  virtual AFx* self_create (size_t srate) = 0;
};


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

  CFxSimpleOverdrive (size_t srate);
  ~CFxSimpleOverdrive();

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);

public slots:

  void dial_gain_valueChanged (int value);
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
  void reset_params (size_t srate, size_t channels);


public slots:

  void spb_ratio_changed (double value);
};


class CFxSimpleLowPass: public AFx
{
  Q_OBJECT

public:

  float gain;

  CFxSimpleLowPass (size_t srate);
 // ~CFxSimpleAmp();

  QLabel *label;

  AFx* self_create (size_t srate);

  size_t execute (float **input, float **output, size_t frames);

public slots:

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



//panning functions

//linear panner, law: -6 dB
inline void pan_linear6 (float &l, float& r, float p)
{
  l = 1 - p;
  r = p;
}


//linear panner, law: 0 dB
inline void pan_linear0 (float &l, float& r, float p)
{
  l = 0.5 + (1 - p);
  r = 0.5 + p;
}


//square root panner, law: -3 dB
inline void pan_sqrt (float &l, float& r, float p)
{
  l = sqrt (1 - p); 
  r = sqrt (p);
}


//sin/cos panner, law: -3 dB
inline void pan_sincos (float &l, float& r, float p)
{
  float pan = 0.5 * M_PI * p;
  l = cos (pan);
  r = sin (pan);
}

#endif // FX_H
