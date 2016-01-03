#ifndef AFX_H
#define AFX_H

#include <cstring>

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>


#include "floatbuffer.h"


enum FxState {
              FXS_STOP,
              FXS_RUN,
              FXS_PAUSE
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

  virtual AFx* self_create (size_t srate) = 0;
};


#endif
