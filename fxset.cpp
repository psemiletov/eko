#include <samplerate.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMouseEvent>
#include <QDial>
#include <QDoubleSpinBox>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#define _USE_MATH_DEFINES

#include <cmath>

#include "db.h"
#include "fxset.h"
#include "utils.h"


QString g_fxpresets_path;

/*

{
  if (! writer)
     return;

  stream->writeStartElement ("effect");
  stream.writeAttribute ("id", typeid (this).name());

  stream->writeStartElement ("param");
  stream.writeAttribute ("name", "dial_gain");
  
  stream.writeAttribute ("type", "floatval");
  
  stream.writeAttribute ("gain", dial_gain->value());

  stream->writeEndElement ("param");

  stream->writeEndElement ("effect");
}
*/


QString CFxSimpleAmp::save_params_to_string()
{
  QString result;
  
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("dial_gain=0:" + QString::number (dial_gain->value()) + ";");
  
  return result;
}


void CFxSimpleAmp::load_params_from_string (const QString &s)
{
//  qDebug() << "CFxSimpleAmp::load_params_from_string - 1";

  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

//почему ls.size() - 1? почему так парсится ls? 
//последний элемент пустой!
  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
            
  QStringList lt = h["dial_gain"].split (",");    
  QStringList lt2 = lt[0].split (":");    
      
  dial_gain->setValue (lt2[1].toDouble());    
  
//  qDebug() << "CFxSimpleAmp::load_params_from_string - 2";
}


CFxSimpleAmp::CFxSimpleAmp()
{
  modulename = tr ("Simple amplifier");
  classname = "CFxSimpleAmp";
  
  wnd_ui->setWindowTitle (modulename);

  set_caption (tr ("<b>Simple amplifier</b>"), tr ("<i>Simple amplifier module</i>"));

  QString qstl = "QWidget {"
    "border-radius: 15px;"
    "background-color: #ffaa00;}";
  
  w_caption->setStyleSheet (qstl);

  presets->update_banks_list (g_fxpresets_path + "/" + classname);

  gain = 1.0f;

  QHBoxLayout *hbl_gain = new QHBoxLayout;

  label = new QLabel (tr ("Input gain: "));

  dial_gain = new QDial;
  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (0, 26);

  dial_gain->setValue (1);

  hbl_gain->addWidget (label);
  hbl_gain->addWidget (dial_gain);
  
  vbl_main->addLayout (hbl_gain);
}


void CFxSimpleAmp::dial_gain_valueChanged (int value)
{
  if (value == 0)
     {
      gain = 1.0f;
      return;
     }

   gain = db2lin (value);
}


void CFxSimpleOverdrive::dial_level_valueChanged (int value)
{
  if (value == 0)
     {
      level = 1.0f;
      return;
     }

   level = db2lin (value);
}


CFxSimpleAmp::~CFxSimpleAmp()
{
//  qDebug() << "~CFxSimpleAmp()";
}


AFx* CFxSimpleAmp::self_create()
{
  return new CFxSimpleAmp;
}


size_t CFxSimpleAmp::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       for (size_t i = 0; i < frames; i++)
            output[ch][i] = input[ch][i] * gain;
      }

  return frames;
}


QString CFxSimpleOverdrive::save_params_to_string()
{
  QString result;
  
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("dial_gain=0:" + QString::number (dial_gain->value()) + ";");
  result += ("dial_level=0:" + QString::number (dial_level->value()) + ";");
  
  return result;
}


void CFxSimpleOverdrive::load_params_from_string (const QString &s)
{
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
  QStringList lt = h["dial_gain"].split (",");    
  QStringList lt2 = lt[0].split (":");    
      
  dial_gain->setValue (lt2[1].toDouble());    
  
  lt = h["dial_level"].split (",");    
  lt2 = lt[0].split (":");    
  
  dial_level->setValue (lt2[1].toDouble());    
}



CFxSimpleOverdrive::CFxSimpleOverdrive()
{
  modulename = tr ("Simple Overdrive");  
  classname = "CFxSimpleOverdrive";
    
  presets->update_banks_list (g_fxpresets_path + "/" + classname);
  
  wnd_ui->setWindowTitle (modulename);

  set_caption (tr ("Simple Overdrive"), tr ("Simple overdrive module"));

  gain = 1.0f;
  level = db2lin (-12.f);

  QHBoxLayout *hbl_gain = new QHBoxLayout;

  
  QLabel *l = new QLabel (tr ("Gain"));
  dial_gain = new QDial;
  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (1, 600);

  dial_gain->setValue (1);

  hbl_gain->addWidget (l);
  hbl_gain->addWidget (dial_gain);
  
  vbl_main->addLayout (hbl_gain);
  
  QHBoxLayout *hbl_level = new QHBoxLayout;

  QLabel *l_level = new QLabel (tr ("Output level: "));

  dial_level = new QDial;
  dial_level->setWrapping (false);
  connect (dial_level, SIGNAL(valueChanged(int)), this, SLOT(dial_level_valueChanged(int)));
  dial_level->setRange (-90, 0);

  dial_level->setValue (-12);

  hbl_level->addWidget (l_level);
  hbl_level->addWidget (dial_level);
  
  vbl_main->addLayout (hbl_level);

}


void CFxSimpleOverdrive::dial_gain_valueChanged (int value)
{
  if (value == 0)
      gain = 1.0f;
  else   
      gain = db2lin (value / 10);
}


CFxSimpleOverdrive::~CFxSimpleOverdrive()
{
  //qDebug() << "~CFxSimpleOverdrive";
}


AFx* CFxSimpleOverdrive::self_create()
{
  return new CFxSimpleOverdrive;
}


size_t CFxSimpleOverdrive::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       for (size_t i = 0; i < frames; i++)
           {
            output[ch][i] = input[ch][i] * gain;

            if (float_greater_than (output[ch][i], 1.0f))
                output[ch][i] = 1.0f;
            else
               if (float_less_than (output[ch][i], -1.0f))
                   output[ch][i] = -1.0f;
                   
           output[ch][i] *= level;        
          /*         
            if (output[ch][i] > 1.0f)
               output[ch][i] = 2 / 3;
            else                 
                if (output[ch][i] < 1.0f)
                  output[ch][i] = (2 / 3) * -1;
                else
                    output[ch][i] = output[ch][i] - (pow (output[ch][i], 3) / 3 );  
*/
           }       
      }

  return frames;
}


CFxDelay::CFxDelay()
{
  classname = "CFxDelay";
  modulename = tr ("Simple Delay");
  
  presets->update_banks_list (g_fxpresets_path + "/" + classname);

  
  fb = new CFloatBuffer (196000, 2);

  wnd_ui->setWindowTitle (modulename);
  set_caption (tr ("Delay"), tr ("Delay module"));

  mixlevel = 1.0f;

  QHBoxLayout *hbl_mixlevel = new QHBoxLayout;

  QLabel *label = new QLabel (tr ("Delayed signal level"));

  spb_mixlevel = new QDoubleSpinBox;
  
  spb_mixlevel->setRange (-26.0f, 0);
  spb_mixlevel->setSingleStep (0.1f);
  spb_mixlevel->setValue (-6.0f);
  connect (spb_mixlevel, SIGNAL(valueChanged (double )), this, SLOT(spb_mixlevel_changed (double )));

  hbl_mixlevel->addWidget (label);
  hbl_mixlevel->addWidget (spb_mixlevel);
  
  
  QHBoxLayout *hbl_time = new QHBoxLayout;

  label = new QLabel (tr ("Time (seconds)"));

  spb_time = new QDoubleSpinBox;
  spb_time->setRange (0.001, 6.000f);
  spb_time->setSingleStep (0.010f);
  spb_time->setValue (0.5f);
  spb_time->setDecimals (3); 
  connect (spb_time, SIGNAL(valueChanged (double )), this, SLOT(spb_time_changed (double )));

  hbl_time->addWidget (label);
  hbl_time->addWidget (spb_time);
  
  
  vbl_main->addLayout (hbl_mixlevel);
  vbl_main->addLayout (hbl_time);
}


QString CFxDelay::save_params_to_string()
{
  QString result;
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("spb_mixlevel=0:" + QString::number (spb_mixlevel->value()) + ";");
  result += ("spb_time=0:" + QString::number (spb_time->value()) + ";");
  
  return result;
}


void CFxDelay::load_params_from_string (const QString &s)
{
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
  QStringList lt = h["spb_mixlevel"].split (",");    
  QStringList lt2 = lt[0].split (":");    
      
  spb_mixlevel->setValue (lt2[1].toDouble());    
  
  lt = h["spb_time"].split (",");    
  lt2 = lt[0].split (":");    
  
  spb_time->setValue (lt2[1].toDouble());    
}


void CFxDelay::spb_mixlevel_changed (double value)
{
  mixlevel = db2lin (value);
}


void CFxDelay::spb_time_changed (double value)
{
  delay_msecs = value;
  
  //qDebug() << "delay_msecs: " << delay_msecs; 
  //qDebug() << "fb->samplerate: " << fb->samplerate; 
  
  fb->ringbuffer_set_length (fb->samplerate * delay_msecs);
  //qDebug() << fb->ringbuffer_length;
}


CFxDelay::~CFxDelay()
{
  if (fb)
     delete fb;
}


AFx* CFxDelay::self_create()
{
  return new CFxDelay;
}


size_t CFxDelay::execute (float **input, float **output, size_t frames)
{
  for (size_t i = 0; i < frames; i++)
      {
       for (size_t ch = 0; ch < channels; ch++)
           {    
            fb->buffer[ch][fb->tail] = input[ch][i];
            output[ch][i] = input[ch][i] + (fb->buffer[ch][fb->head] * mixlevel);
           }
        
       fb->ringbuffer_head_inc();
       fb->ringbuffer_tail_inc();
      }
  
  return frames;
}


void CFxDelay::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
 
  if (fb)
     delete fb;
     
  delay_msecs = spb_time->value();
  mixlevel = db2lin (spb_mixlevel->value());
     
  fb = new CFloatBuffer (srate * 6, channels); //6 seconds max delay
  fb->samplerate = srate;
  fb->ringbuffer_set_length (fb->samplerate * delay_msecs);
}


void CFxSimpleFilter::cmb_filter_mode_currentIndexChanged (int index)
{
  filter.mode = index;
}


void CFxSimpleFilter::dsb_cutoff_valueChanged (double d)
{
  filter.set_cutoff ((float) d / samplerate);
}


void CFxSimpleFilter::dsb_reso_valueChanged (double d)
{
  filter.set_resonance (d);
}


CFxSimpleFilter::CFxSimpleFilter()
{
  classname = "CFxSimpleFilter";
  modulename = tr ("Simple Filter");
  
  presets->update_banks_list (g_fxpresets_path + "/" + classname);
  
  wnd_ui->setWindowTitle (modulename);
  set_caption (tr ("Filter"), tr ("Multi-mode filter module"));

  
  QHBoxLayout *h_box_filter_mode = new QHBoxLayout;
  
  QLabel *l_filter_mode = new QLabel (tr ("Mode:"));

  cmb_filter_mode = new QComboBox;
  cmb_filter_mode->addItem (tr ("Lowpass"));
  cmb_filter_mode->addItem (tr ("Highpass"));
  cmb_filter_mode->addItem (tr ("Bandpass"));

  connect (cmb_filter_mode, SIGNAL(currentIndexChanged (int)),
           this, SLOT(cmb_filter_mode_currentIndexChanged (int)));


  h_box_filter_mode->addWidget (l_filter_mode);
  h_box_filter_mode->addWidget (cmb_filter_mode);
  
  vbl_main->addLayout (h_box_filter_mode);
  
  
  QHBoxLayout *h_box_cutoff = new QHBoxLayout;
  
  QLabel *l_cutoff_freq = new QLabel (tr ("Cut-off freq (hZ)"));
  dsb_cutoff_freq = new QDoubleSpinBox;
  h_box_cutoff->addWidget (l_cutoff_freq);
  h_box_cutoff->addWidget (dsb_cutoff_freq);

  dsb_cutoff_freq->setDecimals (2); 
  dsb_cutoff_freq->setRange (1, 48000);
  dsb_cutoff_freq->setSingleStep (0.01);
 
    
  connect (dsb_cutoff_freq, SIGNAL(valueChanged(double)), this, SLOT(dsb_cutoff_valueChanged(double)));
  dsb_cutoff_freq->setValue (22100);
   
  vbl_main->addLayout (h_box_cutoff);

  QHBoxLayout *h_box_reso = new QHBoxLayout;
  
  QLabel *l_dsb_reso = new QLabel (tr ("Resonance"));
  dsb_reso = new QDoubleSpinBox;
  h_box_reso->addWidget (l_dsb_reso);
  h_box_reso->addWidget (dsb_reso);
  
  connect (dsb_reso, SIGNAL(valueChanged(double)), this, SLOT(dsb_reso_valueChanged(double)));
  dsb_reso->setDecimals (3); 
  dsb_reso->setRange (0.001f, 1.0f);
  dsb_reso->setSingleStep (0.001f);
  
  dsb_reso->setValue (0.01f);
   
  vbl_main->addLayout (h_box_reso);
}


QString CFxSimpleFilter::save_params_to_string()
{
  QString result;
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("dsb_cutoff_freq=0:" + QString::number (dsb_cutoff_freq->value()) + ";");
  result += ("dsb_reso=0:" + QString::number (dsb_reso->value()) + ";");
  result += ("cmb_filter_mode=0:" + QString::number (cmb_filter_mode->currentIndex()) + ";");
  
  return result;
}


void CFxSimpleFilter::load_params_from_string (const QString &s)
{
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
  QStringList lt = h["dsb_cutoff_freq"].split (",");    
  QStringList lt2 = lt[0].split (":");    
      
  dsb_cutoff_freq->setValue (lt2[1].toDouble());    
  
  lt = h["dsb_reso"].split (",");    
  lt2 = lt[0].split (":");    
  
  dsb_reso->setValue (lt2[1].toDouble());    
  
  lt = h["cmb_filter_mode"].split (",");    
  lt2 = lt[0].split (":");    
  
  cmb_filter_mode->setCurrentIndex (lt2[1].toInt());
}


AFx* CFxSimpleFilter::self_create()
{
  return new CFxSimpleFilter;
}


size_t CFxSimpleFilter::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
        for (size_t i = 0; i < frames; i++)
          {
           input[ch][i] = filter.process (input[ch][i], ch);
          }
      }

  return frames;
}


void CFxSimpleFilter::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
  filter.reset();
  
  filter.set_cutoff ((float) dsb_cutoff_freq->value() / samplerate);
}


CFxMetaluga::CFxMetaluga()
{
  modulename = tr ("Metaluga (overdrive/dist pedal)");
  classname = "CFxMetaluga";
  
  presets->update_banks_list (g_fxpresets_path + "/" + classname);

  
  wnd_ui->setWindowTitle (tr ("Metaluga"));

  set_caption (tr ("<b>Metaluga</b>"), tr ("<i>Overdrive/distortion pedal</i>"));

  gain = 1.0f;
  level = db2lin (-12.0f);
  drive = 1.0f;
  tone = 1.0f;
  
  filter.mode = 2;
  filter.set_resonance (0.5f);

  QHBoxLayout *hbl_gain = new QHBoxLayout;
  
  QLabel *l = new QLabel (tr ("Gain"));
  dial_gain = new QDial;
  dial_gain->setNotchesVisible (true);

  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (1, 100);

  dial_gain->setValue (1);

  hbl_gain->addWidget (l);
  hbl_gain->addWidget (dial_gain);
  
  vbl_main->addLayout (hbl_gain);
  
  
  QHBoxLayout *hbl_drive = new QHBoxLayout;
  
  l = new QLabel (tr ("Drive"));
  dial_drive = new QDial;
  dial_drive->setNotchesVisible (true);

  dial_drive->setWrapping (false);
  connect (dial_drive, SIGNAL(valueChanged(int)), this, SLOT(dial_drive_valueChanged(int)));
  dial_drive->setRange (1, 26);

  dial_drive->setValue (1);

  hbl_drive->addWidget (l);
  hbl_drive->addWidget (dial_drive);
  
  vbl_main->addLayout (hbl_drive);
  
  
  QHBoxLayout *hbl_tone = new QHBoxLayout;
  
  l = new QLabel (tr ("Tone"));
  dial_tone = new QDial;
  dial_tone->setNotchesVisible (true);

  dial_tone->setWrapping (false);
  connect (dial_tone, SIGNAL(valueChanged(int)), this, SLOT(dial_tone_valueChanged(int)));
 
  dial_tone->setRange (1, 100);
  dial_tone->setValue (50);

  hbl_tone->addWidget (l);
  hbl_tone->addWidget (dial_tone);
  
  vbl_main->addLayout (hbl_tone);


  QHBoxLayout *hbl_level = new QHBoxLayout;

  QLabel *l_level = new QLabel (tr ("Output level"));

  dial_level = new QDial;
  dial_level->setWrapping (false);
  connect (dial_level, SIGNAL(valueChanged(int)), this, SLOT(dial_level_valueChanged(int)));
  dial_level->setRange (-90, 0);

  dial_level->setValue (-12);

  dial_level->setNotchesVisible (true);

  hbl_level->addWidget (l_level);
  hbl_level->addWidget (dial_level);
  
  vbl_main->addLayout (hbl_level);

  QString qstl = "QWidget#w_caption {"
    "border-radius: 15px;"
    "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "stop:0 #000000, stop:1 #363636);}";
  
  w_caption->setStyleSheet (qstl);  
  l_caption->setStyleSheet ("color: white;");  
  l_subcaption->setStyleSheet ("color: white;");  
}


QString CFxMetaluga::save_params_to_string()
{
  QString result;
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("dial_gain=0:" + QString::number (dial_gain->value()) + ";");
  result += ("dial_drive=0:" + QString::number (dial_drive->value()) + ";");
  result += ("dial_tone=0:" + QString::number (dial_tone->value()) + ";");
  result += ("dial_level=0:" + QString::number (dial_level->value()) + ";");
  
  return result;
}


void CFxMetaluga::load_params_from_string (const QString &s)
{
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
  QStringList lt = h["dial_gain"].split (",");    
  QStringList lt2 = lt[0].split (":");    
  dial_gain->setValue (lt2[1].toDouble());    
  
  lt = h["dial_drive"].split (",");    
  lt2 = lt[0].split (":");    
  dial_drive->setValue (lt2[1].toDouble());    
  
  lt = h["dial_tone"].split (",");    
  lt2 = lt[0].split (":");    
  dial_tone->setValue (lt2[1].toDouble());    

  lt = h["dial_level"].split (",");    
  lt2 = lt[0].split (":");    
  dial_level->setValue (lt2[1].toDouble());    
}


void CFxMetaluga::dial_gain_valueChanged (int value)
{
  if (value == 0)
      gain = 1.0f;
  else   
      gain = db2lin (value);
}


void CFxMetaluga::dial_drive_valueChanged (int value)
{
/*
this code is based on "A Guitar Overdrive/Distortion Effect of Digital Signal Processing"
algoritm from the article written by Cheng-Hao Chang
*/

  float a = sin (((drive + 1) / 101) * (M_PI / 2));
  float k = 2 * atan (a) / (1 - a);
  drive = (1 + k) * value / (1 + k * abs (value));
}


void CFxMetaluga::dial_tone_valueChanged (int value)
{
 float val = scale_val (value, 1, 100, 1000, 21000);
 filter.set_cutoff (val / samplerate);
}


CFxMetaluga::~CFxMetaluga()
{
  //qDebug() << "~CFxSimpleOverdrive";
}


AFx* CFxMetaluga::self_create()
{
  return new CFxMetaluga;
}


size_t CFxMetaluga::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       for (size_t i = 0; i < frames; i++)
           {
            output[ch][i] = input[ch][i] * gain;
            
            output[ch][i] = (float) (atan(output[ch][i] * drive) / atan(drive));
            
            output[ch][i] *= drive;

            output[ch][i] = filter.process (output[ch][i], ch);

            output[ch][i] *= level;  
           }       
      }

  return frames;
}


void CFxMetaluga::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
  filter.reset();
  
  float val = scale_val (dial_tone->value(), 1, 100, 1000, 21000);
  filter.set_cutoff (val / samplerate);
}


void CFxMetaluga::dial_level_valueChanged (int value)
{
  if (value == 0)
     {
      level = 1.0f;
      return;
     }

   level = db2lin (value);
}


CFxJest::CFxJest()
{
  modulename = tr ("Jest' (overdrive/dist)");
  classname = "CFxJest";
  
  presets->update_banks_list (g_fxpresets_path + "/" + classname);

  
  wnd_ui->setWindowTitle (tr ("Jest'"));

  set_caption (tr ("<b>Jest'</b>"), tr ("<i>Soviet peregrooz pedal</i>"));

  gain = 1.0f;
  level = db2lin (-12.0f);
  drive = 1.0f;
  tone = 1.0f;
  
  filter.mode = 2;
  filter.set_resonance (0.5f);

  QHBoxLayout *hbl_gain = new QHBoxLayout;
  
  QLabel *l = new QLabel (tr ("Gain"));
  dial_gain = new QDial;
  dial_gain->setNotchesVisible (true);

  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (1, 100);

  dial_gain->setValue (1);

  hbl_gain->addWidget (l);
  hbl_gain->addWidget (dial_gain);
  
  vbl_main->addLayout (hbl_gain);
  
  
  QHBoxLayout *hbl_drive = new QHBoxLayout;
  
  l = new QLabel (tr ("Drive"));
  dial_drive = new QDial;
  dial_drive->setNotchesVisible (true);

  dial_drive->setWrapping (false);
  connect (dial_drive, SIGNAL(valueChanged(int)), this, SLOT(dial_drive_valueChanged(int)));
  dial_drive->setRange (1, 26);

  dial_drive->setValue (1);

  hbl_drive->addWidget (l);
  hbl_drive->addWidget (dial_drive);
  
  vbl_main->addLayout (hbl_drive);
    
  QHBoxLayout *hbl_tone = new QHBoxLayout;
  
  l = new QLabel (tr ("Tone"));
  dial_tone = new QDial;
  dial_tone->setNotchesVisible (true);

  dial_tone->setWrapping (false);
  connect (dial_tone, SIGNAL(valueChanged(int)), this, SLOT(dial_tone_valueChanged(int)));
  
  dial_tone->setRange (1, 100);
  dial_tone->setValue (50);


  hbl_tone->addWidget (l);
  hbl_tone->addWidget (dial_tone);
  
  vbl_main->addLayout (hbl_tone);


  QHBoxLayout *hbl_level = new QHBoxLayout;

  QLabel *l_level = new QLabel (tr ("Output level"));

  dial_level = new QDial;
  dial_level->setWrapping (false);
  connect (dial_level, SIGNAL(valueChanged(int)), this, SLOT(dial_level_valueChanged(int)));
  dial_level->setRange (-90, 0);

  dial_level->setValue (-12);

  dial_level->setNotchesVisible (true);

  hbl_level->addWidget (l_level);
  hbl_level->addWidget (dial_level);
  
  vbl_main->addLayout (hbl_level);

  QString qstl = "QWidget#w_caption {"
    "border-radius: 15px;"
    "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "stop:0 #aa5500, stop:1 #aa0000);}";
  
  
  w_caption->setStyleSheet (qstl);  
}


QString CFxJest::save_params_to_string()
{
  QString result;
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("dial_gain=0:" + QString::number (dial_gain->value()) + ";");
  result += ("dial_drive=0:" + QString::number (dial_drive->value()) + ";");
  result += ("dial_tone=0:" + QString::number (dial_tone->value()) + ";");
  result += ("dial_level=0:" + QString::number (dial_level->value()) + ";");
  
  return result;
}


void CFxJest::load_params_from_string (const QString &s)
{
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
  QStringList lt = h["dial_gain"].split (",");    
  QStringList lt2 = lt[0].split (":");    
  dial_gain->setValue (lt2[1].toDouble());    
  
  lt = h["dial_drive"].split (",");    
  lt2 = lt[0].split (":");    
  dial_drive->setValue (lt2[1].toDouble());    
  
  lt = h["dial_tone"].split (",");    
  lt2 = lt[0].split (":");    
  dial_tone->setValue (lt2[1].toDouble());    

  lt = h["dial_level"].split (",");    
  lt2 = lt[0].split (":");    
  dial_level->setValue (lt2[1].toDouble());    
}



void CFxJest::dial_gain_valueChanged (int value)
{
  if (value == 0)
      gain = 1.0f;
  else   
      gain = db2lin (value);
}


void CFxJest::dial_drive_valueChanged (int value)
{
  filter.set_resonance (scale_val (value, 1, 26, 0.001, 0.999f));
}


void CFxJest::dial_tone_valueChanged (int value)
{
  float val = scale_val (dial_tone->value(), 1, 100, 1000, 21000);
  filter.set_cutoff (val / samplerate);
}


AFx* CFxJest::self_create()
{
  return new CFxJest;
}


size_t CFxJest::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       for (size_t i = 0; i < frames; i++)
           {
            output[ch][i] = input[ch][i] * gain;
            output[ch][i] = (float) (atan(output[ch][i] * drive) / atan(drive));
            output[ch][i] = filter.process (output[ch][i], ch);
            output[ch][i] *= level;  
           }       
      }

  return frames;
}


void CFxJest::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
  filter.reset();
  float val = scale_val (dial_tone->value(), 1, 100, 1000, 21000);
  filter.set_cutoff (val / samplerate);

}


void CFxJest::dial_level_valueChanged (int value)
{
  if (value == 0)
     {
      level = 1.0f;
      return;
     }

   level = db2lin (value);
}


void CFxVynil::dial_scratches_amount_valueChanged (int value)
{
 cutoff = scale_val (value, 1, 100, 96000, 500);
}


CFxVynil::CFxVynil()
{
  modulename = tr ("Vynil Taste");
  classname = "CFxVynil";

  presets->update_banks_list (g_fxpresets_path + "/" + classname);

  
  wnd_ui->setWindowTitle (modulename);
  set_caption (tr ("Vynil Taste"), tr ("Scratches generator module"));

  QHBoxLayout *hbl_scratches = new QHBoxLayout;
  
  QLabel *l = new QLabel (tr ("Amount"));
  dial_scratches_amount = new QDial;
  dial_scratches_amount->setNotchesVisible (true);

  dial_scratches_amount->setWrapping (false);
  connect (dial_scratches_amount, SIGNAL(valueChanged(int)), this, SLOT(dial_scratches_amount_valueChanged(int)));
  
  dial_scratches_amount->setRange (1, 100);
  dial_scratches_amount->setValue (50);


  hbl_scratches->addWidget (l);
  hbl_scratches->addWidget (dial_scratches_amount);
  
  vbl_main->addLayout (hbl_scratches);

  QHBoxLayout *hbl_mixlevel = new QHBoxLayout;

  QLabel *label = new QLabel (tr ("Scratches signal level"));

  spb_mixlevel = new QDoubleSpinBox;
  
  spb_mixlevel->setRange (-26.0f, 0);
  spb_mixlevel->setSingleStep (0.1f);
  
  connect (spb_mixlevel, SIGNAL(valueChanged (double )), this, SLOT(spb_mixlevel_changed (double )));

  spb_mixlevel->setValue (-16.0f);
  
  hbl_mixlevel->addWidget (label);
  hbl_mixlevel->addWidget (spb_mixlevel);

  vbl_main->addLayout (hbl_mixlevel);
  
  
   QString qstl = "QWidget#w_caption {"
    "border-radius: 15px;"
    "background:  qradialgradient(cx:0.5, cy:0.5, radius: 0.8, fx:0.5, fy:0.5, stop:0 black, stop:0.1 grey, stop:0.7 black, stop:1 black) }";
                
  
  w_caption->setStyleSheet (qstl);  

  l_caption->setStyleSheet ("color: white;");  
  l_subcaption->setStyleSheet ("color: white;");  
}


AFx* CFxVynil::self_create()
{
  return new CFxVynil;
}


size_t CFxVynil::execute (float **input, float **output, size_t frames)
{
 
  for (size_t ch = 0; ch < channels; ch++)
      {
        for (size_t i = 0; i < frames; i++)
          {
           float f = input[ch][i];
           f = (float)f / (f + float (1 / cutoff));
           f *= mixlevel;
           input[ch][i] += f;
          }
      }

  return frames;
}


void CFxVynil::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
}


void CFxVynil::spb_mixlevel_changed (double value)
{
  mixlevel = db2lin (value);
}


QString CFxVynil::save_params_to_string()
{
  QString result;
  //format is: paramname=frame_number1:value,frame_numberN:valueN;
  //it is designed to save automation pairs time:value
  result += ("spb_mixlevel=0:" + QString::number (spb_mixlevel->value()) + ";");
  result += ("dial_scratches_amount=0:" + QString::number (dial_scratches_amount->value()) + ";");
  
  return result;
}


void CFxVynil::load_params_from_string (const QString &s)
{
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size() - 1; i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
  QStringList lt = h["spb_mixlevel"].split (",");    
  QStringList lt2 = lt[0].split (":");    
  spb_mixlevel->setValue (lt2[1].toDouble());    
  
  lt = h["dial_scratches_amount"].split (",");    
  lt2 = lt[0].split (":");    
  dial_scratches_amount->setValue (lt2[1].toDouble());    
}

