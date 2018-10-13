#ifndef AFX_H
#define AFX_H

#include <cstring>

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>


#include "floatbuffer.h"
#include "fxpresets.h"


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
  
  CFxPresets *presets;
  
  QWidget *wnd_ui;
  QVBoxLayout *vbl_main;
  
  QWidget *w_caption;
  QLabel *l_caption;
  QLabel *l_subcaption;
  
  QString classname;
  QString modulename;

  AFx();
  
  /*virtual*/ ~AFx();

  virtual size_t execute (float **input, float **output, size_t frames) = 0;
  
  //virtual void state_save_xml (QXmlStreamWriter *writer) = 0;
  
  virtual QString save_params_to_string() = 0;
  virtual void load_params_from_string (const QString &s) = 0;
  
  virtual void set_state (FxState s);
  virtual void reset_params (size_t srate, size_t ch);

  void set_caption (const QString &capt, const QString &subcapt);

  void show_ui();

  virtual AFx* self_create() = 0;

  static QString get_modulename() {return QString ("AFx");};
 
public slots:

  void slot_preset_changed (const QString &text);
  void slot_save_request();
 
};

#endif
