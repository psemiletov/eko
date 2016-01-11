#include <samplerate.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMouseEvent>
#include <QDial>
#include <QDoubleSpinBox>
#include <QDrag>
#include <QMimeData>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#define _USE_MATH_DEFINES

#include <cmath>

#include "fxset.h"
#include "utils.h"

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
  QStringList ls = s.split (";");
  QHash <QString, QString> h;
  //parsing

  for (int i = 0; i < ls.size(); i++)
      {
       QStringList lt = ls[i].split ("=");
       h[lt[0]] = lt[1];
      }  
      
      
  QStringList lt = h["dial_gain"].split (",");    
  QStringList lt2 = lt[0].split (":");    
      
  dial_gain->setValue (lt2[1].toDouble());    
}


CFxSimpleAmp::CFxSimpleAmp()
{
  name = tr ("Simple amplifier");
  
  wnd_ui->setWindowTitle (tr ("Simple Amp"));

  set_caption (tr ("<b>Simple amplifier</b>"), tr ("<i>Simple amplifier module</i>"));

  QString qstl = "QWidget {"
    "border-radius: 15px;"
    "background-color: #ffaa00;}";
  
  w_caption->setStyleSheet (qstl);

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

  for (int i = 0; i < ls.size(); i++)
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
  name = "FxSimpleOverdrive";
  
  wnd_ui->setWindowTitle (tr ("Simple Overdrive"));

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
  name = "CFxDelay";
  
  fb = new CFloatBuffer (96000, 2);


  wnd_ui->setWindowTitle (tr ("Simple Delay"));
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


void CFxDelay::spb_mixlevel_changed (double value)
{
  mixlevel = db2lin (value);
}


void CFxDelay::spb_time_changed (double value)
{
  delay_msecs = value;
  
  qDebug() << "delay_msecs: " << delay_msecs; 
  qDebug() << "fb->samplerate: " << fb->samplerate; 
  
  fb->ringbuffer_set_length (fb->samplerate * delay_msecs);
  qDebug() << fb->ringbuffer_length;
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
  name = "CFxSimpleFilter";
  
  wnd_ui->setWindowTitle (tr ("Simple Filter"));
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


AFx* CFxSimpleFilter::self_create()
{
  return new CFxSimpleFilter;
}


/* C function implementing the simplest lowpass:
 *
 *      y(n) = x(n) + x(n-1)
 *
 */
float simplp (float *x, float *y,
              size_t M, float xm1)
{
  y[0] = x[0] + xm1;
  
  for (size_t n = 1; n < M ; n++) 
       {
        y[n] =  x[n]  + x[n-1];
       } 
       
  return x[M-1];
}

/*
size_t CFxSimpleFilter::execute (float **input, float **output, size_t frames)
{
  float xm1 = 0.0f;
  float M = frames;

  for (size_t ch = 0; ch < channels; ch++)
      {
      float xm1 = 0.0f;
  
      // for (size_t i = 0; i < frames; i++)
            xm1 = simplp (input[ch], output[ch], M, xm1);
      }

  return frames;
}
*/

#define CUTOFF 12000

float del[2]={0.f, 0.f}, del1[2]={0.f,0.f};


float lowpass(float* sig, float freq, float *del,
int vecsize, float sr){
double costh, coef;
costh = 2. - cos(2* M_PI * freq/sr);
coef = sqrt(costh*costh - 1.) - costh;
for(int i =0; i < vecsize; i++){
sig[i] = (float) (sig[i]*(1 + coef) - *del*coef);
*del = sig[i];
}
return *sig;
}


size_t CFxSimpleFilter::execute (float **input, float **output, size_t frames)
{
 
  for (size_t ch = 0; ch < channels; ch++)
      {
        //lowpass (input[ch], 20, del, frames, samplerate);
         
        for (size_t i = 0; i < frames; i++)
          {
           input[ch][i] = filter.process (input[ch][i], ch);

         
          }
      }

  return frames;

}


/*


One pole LP and HP

References : Posted by Bram
Code :
LP:
recursion: tmp = (1-p)*in + p*tmp with output = tmp
coefficient: p = (2-cos(x)) - sqrt((2-cos(x))^2 - 1) with x = 2*pi*cutoff/samplerate
coeficient approximation: p = (1 - 2*cutoff/samplerate)^2

HP:
recursion: tmp = (p-1)*in - p*tmp with output = tmp
coefficient: p = (2+cos(x)) - sqrt((2+cos(x))^2 - 1) with x = 2*pi*cutoff/samplerate
coeficient approximation: p = (2*cutoff/samplerate)^2


*/


/*

LP and HP filter

Type : biquad, tweaked butterworth
References : Posted by Patrice Tarrabia
Code :
r  = rez amount, from sqrt(2) to ~ 0.1
f  = cutoff frequency
(from ~0 Hz to SampleRate/2 - though many
synths seem to filter only  up to SampleRate/4)

The filter algo:
out(n) = a1 * in + a2 * in(n-1) + a3 * in(n-2) - b1*out(n-1) - b2*out(n-2)

Lowpass:
      c = 1.0 / tan(pi * f / sample_rate);

      a1 = 1.0 / ( 1.0 + r * c + c * c);
      a2 = 2* a1;
      a3 = a1;
      b1 = 2.0 * ( 1.0 - c*c) * a1;
      b2 = ( 1.0 - r * c + c * c) * a1;

Hipass:
      c = tan(pi * f / sample_rate);

      a1 = 1.0 / ( 1.0 + r * c + c * c);
      a2 = -2*a1;
      a3 = a1;
      b1 = 2.0 * ( c*c - 1.0) * a1;
      b2 = ( 1.0 - r * c + c * c) * a1;

*/


void CFxSimpleFilter::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
  filter.reset();
  
  filter.set_cutoff ((float) dsb_cutoff_freq->value() / samplerate);
  
}


CFxMetaluga::CFxMetaluga()
{
  name = tr ("Metaluga (overdrive/dist pedal)");
  
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
  QDial *dial_gain = new QDial;
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
  QDial *dial_drive = new QDial;
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

  QDial *dial_level = new QDial;
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


void CFxMetaluga::dial_gain_valueChanged (int value)
{
  if (value == 0)
      gain = 1.0f;
  else   
      gain = db2lin (value);
}


void CFxMetaluga::dial_drive_valueChanged (int value)
{
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
  name = tr ("Jest' (overdrive/dist)");
  
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
  QDial *dial_gain = new QDial;
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
  QDial *dial_drive = new QDial;
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

  QDial *dial_level = new QDial;
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
