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


#include "fx.h"
#include "utils.h"
//#include "3pass_eq.h"


void CRingbuffer::put (float f)
{
  head++;

  if (head >= samples_max)
     head = 0;

  current_size++;

  buffer[head] = f;
}


float CRingbuffer::get()
{
  if (current_size == 0)
     return ret_value;

  tail++;

  if (tail >= samples_max)
     tail = 0;

  current_size--;

  return buffer[tail];
}


CRingbuffer::CRingbuffer (size_t samples, float def_value)
{
  ret_value = def_value;
  buffer = 0;
  samples_max = 0;
  current_size = 0;
  head = 0;
  tail = 0;

  prepare_buffer (samples);
}


void CRingbuffer::prepare_buffer (size_t samples)
{
  qDebug() << "CRingbuffer::prepare_buffer " << samples;

  if (buffer)
     delete buffer;

  buffer = new float [samples]; 
  samples_max = samples;

  qDebug() << "CRingbuffer::prepare_buffer - done ";
}


CRingbuffer::~CRingbuffer()
{
  if (buffer)
     delete buffer;
}


AFx::AFx (size_t srate)
{
  bypass = false;
  realtime = true;
  ui_visible = false;
  float_buffer = 0;
 
  state = FXS_STOP;
  name = "AFx";

  samplerate = srate;
  channels = 1;//chann;
  
  wnd_ui = new QWidget();

  vbl_main = new QVBoxLayout;
  wnd_ui->setLayout (vbl_main);

  w_caption = new QWidget; 
  QVBoxLayout *vbl_caption = new QVBoxLayout;
  w_caption->setLayout (vbl_caption);
  
  vbl_main->addWidget (w_caption);
  
  l_caption = new QLabel;
  l_subcaption = new QLabel;

  vbl_caption->addWidget (l_caption);
  vbl_caption->addWidget (l_subcaption);

  QString qstl = "QWidget#w_caption{"
    "border-radius: 15px;"
    "background-color: grey;}";
  
  
  w_caption->setObjectName ("w_caption");
  w_caption->setStyleSheet (qstl);
}


void AFx::set_caption (const QString &capt, const QString &subcapt)
{
  l_caption->setText (capt);
  l_subcaption->setText (subcapt);
}


AFx::~AFx()
{
  if (wnd_ui)
     {
      wnd_ui->close();
      delete wnd_ui;
     }
}


void AFx::show_ui()
{
  if (wnd_ui)
     wnd_ui->setVisible (! wnd_ui->isVisible());
}


CFxList::CFxList()
{
  list.append (new CFxSimpleAmp (1));
  list.append (new CFxSimpleOverdrive (1));
  list.append (new CFxPitchShift (1));
  list.append (new CFxSimpleFilter (1));
  list.append (new CFxClassicOverdrive (1));
  
  
}


CFxList::~CFxList()
{
  foreach (AFx *f, list) 
          {
           delete f; 
          }
}
 

AFx *CFxList::find_by_name (const QString &fxname)
{
  for (int i = 0; i < list.size(); i++)
      {
       if (list[i]->name == fxname) 
          return list[i];
      }    

  return 0;
}



QStringList CFxList::names()
{
  QStringList l;
  foreach (AFx *f, list)
          l.append (f->name);
  return l;        
}


CFxSimpleAmp::CFxSimpleAmp (size_t srate): AFx (srate)
{
  name = "FxSimpleAmp";

  wnd_ui->setWindowTitle (tr ("Simple Amp"));

  set_caption (tr ("Simple Amp"), tr ("Simple Ampifier"));

  QString qstl = "QWidget {"
    "border-radius: 15px;"
    "background-color: #ffaa00;}";
  
  w_caption->setStyleSheet (qstl);


  gain = 1.0f;

  QHBoxLayout *hbl_gain = new QHBoxLayout;

  label = new QLabel (tr ("Input gain: "));

  QDial *dial_gain = new QDial;
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

  // label->setText (tr ("Volume: %1 dB").arg (value));
}


void CFxSimpleOverdrive::dial_level_valueChanged (int value)
{
  if (value == 0)
     {
      level = 1.0f;
      return;
     }

   level = db2lin (value);

   //label->setText (tr ("Volume: %1 dB").arg (value));
}


CFxSimpleAmp::~CFxSimpleAmp()
{
//  qDebug() << "~CFxSimpleAmp()";
}


AFx* CFxSimpleAmp::self_create (size_t srate)
{
  return new CFxSimpleAmp (srate);
}


size_t CFxSimpleAmp::execute (float **input, float **output, size_t frames)
{
  qDebug() << "CFxSimpleAmp::execute ";
  for (size_t ch = 0; ch < channels; ch++)
      {
       for (size_t i = 0; i < frames; i++)
            output[ch][i] = input[ch][i] * gain;
      }

  return frames;
}

/*
size_t CFxSimpleEQ::execute (float **input, float **output, size_t frames)
{
  
  if (channels == 1)
     for (size_t i = 0; i < frames; i++)
          output[0][i] = do_3band (&eq_state_ch00, input[0][i]);

  if (channels == 2)
     {
      for (size_t i = 0; i < frames; i++)
          output[1][i] = do_3band (&eq_state_ch01, input[1][i]);
     }

  return frames;
}


CFxSimpleEQ::CFxSimpleEQ (size_t srate): AFx (srate)
{
  name = "CFxSimpleEQ";
  wnd_ui = new QWidget();

  wnd_ui->setWindowTitle (tr ("EQ"));

  eq_state_ch00.lg = 1.0f;
  eq_state_ch00.mg = 1.0f;
  eq_state_ch00.hg = 1.0f;

  eq_state_ch01.lg = 1.0f;
  eq_state_ch01.mg = 1.0f;
  eq_state_ch01.hg = 1.0f;

  init_3band_state (&eq_state_ch00, 880, 5000, samplerate * 100);
  init_3band_state (&eq_state_ch01, 880, 5000, samplerate * 100);

//  init_3band_state (&eq_state_ch00, 880, 5000, srate * 100);
//  init_3band_state (&eq_state_ch01, 880, 5000, srate * 100);


  QVBoxLayout *v_box = new QVBoxLayout;
  wnd_ui->setLayout (v_box);

  QLabel *l = new QLabel (tr ("Low"));
  QDial *dial_low = new QDial;
  dial_low->setWrapping (false);
  connect (dial_low, SIGNAL(valueChanged(int)), this, SLOT(dial_low_valueChanged(int)));
  dial_low->setRange (-600, 600);

  dial_low->setValue (0);

  v_box->addWidget (l);
  v_box->addWidget (dial_low);


  l = new QLabel (tr ("Mid"));
  QDial *dial_mid = new QDial;
  dial_mid->setWrapping (false);
  connect (dial_mid, SIGNAL(valueChanged(int)), this, SLOT(dial_mid_valueChanged(int)));
  dial_mid->setRange (-600, 600);
  dial_mid->setValue (0);

  v_box->addWidget (l);
  v_box->addWidget (dial_mid);


  l = new QLabel (tr ("Hi"));
  QDial *dial_hi = new QDial;
  dial_hi->setWrapping (false);
  connect (dial_hi, SIGNAL(valueChanged(int)), this, SLOT(dial_hi_valueChanged(int)));
  dial_hi->setRange (-600, 600);
  dial_hi->setValue (0);

  v_box->addWidget (l);
  v_box->addWidget (dial_hi);
}


CFxSimpleEQ::~CFxSimpleEQ()
{
//  qDebug() << "~CFxSimpleEQ()";
}


AFx* CFxSimpleEQ::self_create (size_t srate)
{
  return new CFxSimpleEQ (srate);
}


void CFxSimpleEQ::dial_low_valueChanged (int value)
{
  float f = 1.0f;

  if (value == 0)
     {
      eq_state_ch00.lg = 1.0f;
      eq_state_ch01.lg = 1.0f;
      return;
     }

   f = db2lin ((float) value / 100);

   eq_state_ch00.lg = f;
   eq_state_ch01.lg = f;
}


void CFxSimpleEQ::dial_mid_valueChanged (int value)
{
  float f = 1.0f;

  if (value == 0)
     {
      eq_state_ch00.mg = 1.0f;
      eq_state_ch01.mg = 1.0f;

      return;
     }

   f = db2lin ((float) value / 100);

   eq_state_ch00.mg = f;
   eq_state_ch01.mg = f;
}


void CFxSimpleEQ::dial_hi_valueChanged (int value)
{
  float f = 1.0f;

  if (value == 0)
     {
      eq_state_ch00.hg = 1.0f;
      eq_state_ch01.hg = 1.0f;
      return;
     }

   f = db2lin ((float) value / 100);

   eq_state_ch00.hg = f;
   eq_state_ch01.hg = f;
}
*/

CFxSimpleOverdrive::CFxSimpleOverdrive (size_t srate): AFx (srate)
{
  name = "FxSimpleOverdrive";
  
  wnd_ui->setWindowTitle (tr ("Simple Overdrive"));

  set_caption (tr ("Simple Overdrive"), tr ("Simple overdrive module"));

  gain = 1.0f;
  level = db2lin (-12.f);

  QHBoxLayout *hbl_gain = new QHBoxLayout;

  
  QLabel *l = new QLabel (tr ("Gain"));
  QDial *dial_gain = new QDial;
  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (1, 600);

  dial_gain->setValue (1);

  hbl_gain->addWidget (l);
  hbl_gain->addWidget (dial_gain);
  
  vbl_main->addLayout (hbl_gain);
  
  QHBoxLayout *hbl_level = new QHBoxLayout;

  QLabel *l_level = new QLabel (tr ("Output level: "));

  QDial *dial_level = new QDial;
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


AFx* CFxSimpleOverdrive::self_create (size_t srate)
{
  return new CFxSimpleOverdrive (srate);
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


void AFx::set_state (FxState s)
{
  state = s;
}


CFxPitchShift::CFxPitchShift (size_t srate): AFx (srate)
{
  name = "FxPitchShift";
  
  fb = new CFloatBuffer (4096, 8);

  wnd_ui->setWindowTitle (tr ("Simple Pitchshifter"));
  set_caption (tr ("Pitchshifter"), tr ("Pitchshifter module"));

  ratio = 1.0;

  QLabel *label = new QLabel (tr ("Ratio: 1.0"));

  QDoubleSpinBox *spb_ratio = new QDoubleSpinBox;
  spb_ratio->setRange (-12, 12);
  spb_ratio->setSingleStep (0.1);
  spb_ratio->setValue (1.0);
  connect (spb_ratio, SIGNAL(valueChanged (double )), this, SLOT(spb_ratio_changed (double )));

  vbl_main->addWidget (label);
  vbl_main->addWidget (spb_ratio);
}


void CFxPitchShift::spb_ratio_changed (double value)
{
  ratio = value;
}


CFxPitchShift::~CFxPitchShift()
{
  if (fb)
     delete fb;
}


AFx* CFxPitchShift::self_create (size_t srate)
{
  return new CFxPitchShift (srate);
}


size_t CFxPitchShift::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       SRC_DATA data;

       data.src_ratio = ratio;

       data.input_frames = frames;
       data.output_frames = frames;

       size_t bsize = frames * sizeof (float);
       
       memset (fb->buffer[ch], 0, bsize);
       
       memcpy (fb->buffer[ch], input[ch], bsize);

       data.data_in = fb->buffer[ch];
       data.data_out = output[ch];

       int q;

       if (realtime)
          q = SRC_LINEAR;
       else
           q = SRC_SINC_BEST_QUALITY;

       int error = src_simple (&data, q, channels);
       if (error)
          {
           qDebug() << src_strerror (error);
           //delete data_in;
           return 0;
          }

       //delete [] data_in;
      }
  
  return frames;
}


void AFx::reset_params (size_t srate, size_t chann)
{
  qDebug() << "AFx::reset_params";

  samplerate = srate;
  channels = chann;
}


void CFxPitchShift::reset_params (size_t srate, size_t ch)
{
  qDebug() << "CFxPitchShift::reset_params";
  AFx::reset_params (srate, ch);
 
  if (fb)
     delete fb;
     
  fb = new CFloatBuffer (4096, channels);
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


CFxSimpleFilter::CFxSimpleFilter (size_t srate): AFx (srate)
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


AFx* CFxSimpleFilter::self_create (size_t srate)
{
  return new CFxSimpleFilter (srate);
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
}


CFxClassicOverdrive::CFxClassicOverdrive (size_t srate): AFx (srate)
{
  name = "FxClassicOverdrive";
  
  wnd_ui->setWindowTitle (tr ("Classic Overdrive"));

  set_caption (tr ("<b>Classic Oderdrive</b>"), tr ("<i>Classic overdrive module</i>"));

  gain = 1.0f;
  level = db2lin (-12.0f);
  drive = 1.0f;
  tone = 1.0f;

  QHBoxLayout *hbl_gain = new QHBoxLayout;
  
  QLabel *l = new QLabel (tr ("Gain"));
  QDial *dial_gain = new QDial;
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
  dial_drive->setWrapping (false);
  connect (dial_drive, SIGNAL(valueChanged(int)), this, SLOT(dial_drive_valueChanged(int)));
  dial_drive->setRange (1, 26);

  dial_drive->setValue (1);

  hbl_drive->addWidget (l);
  hbl_drive->addWidget (dial_drive);
  
  vbl_main->addLayout (hbl_drive);
  
  
  QHBoxLayout *hbl_tone = new QHBoxLayout;
  
  l = new QLabel (tr ("Tone"));
  QDial *dial_tone = new QDial;
  dial_tone->setWrapping (false);
  connect (dial_tone, SIGNAL(valueChanged(int)), this, SLOT(dial_tone_valueChanged(int)));
  dial_tone->setRange (1000, 21000);

  dial_tone->setValue (11500);

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

  hbl_level->addWidget (l_level);
  hbl_level->addWidget (dial_level);
  
  vbl_main->addLayout (hbl_level);

  QString qstl = "QWidget#w_caption {"
    "border-radius: 15px;"
    "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                "stop:0 #aa5500, stop:1 #aa0000);}";
  
  
  w_caption->setStyleSheet (qstl);  
  
   
  
  
}


void CFxClassicOverdrive::dial_gain_valueChanged (int value)
{
//  gain = ((float)(value / 100) * 100) + 1;

/*
  gain = (float) value / 100;
  gain *= 100;
  gain += 1.0;
*/
  if (value == 0)
      gain = 1.0f;
  else   
      gain = db2lin (value);


   qDebug() << "gain:" << gain;

}


void CFxClassicOverdrive::dial_drive_valueChanged (int value)
{
 /* if (value == 0)
      gain = 1.0f;
  else   
      gain = db2lin (value / 10);*/
      
  float a = sin (((drive + 1) / 101) * (M_PI / 2));
  
  qDebug() << "a:" << a;
  
  float k = 2 * cos (a) / (1 - a);
  
  qDebug() << "k:" << k;
  
  drive = (1 + k) * (value) / (1 + k * abs (value));
  
  qDebug() << "drive:" << drive;
 
}


void CFxClassicOverdrive::dial_tone_valueChanged (int value)
{
  filter.set_cutoff ((float) value / samplerate);
}


CFxClassicOverdrive::~CFxClassicOverdrive()
{
  //qDebug() << "~CFxSimpleOverdrive";
}


AFx* CFxClassicOverdrive::self_create (size_t srate)
{
  return new CFxClassicOverdrive (srate);
}


size_t CFxClassicOverdrive::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       for (size_t i = 0; i < frames; i++)
           {
            output[ch][i] = input[ch][i] * gain;
            output[ch][i] *= drive;
            output[ch][i] = filter.process (output[ch][i], ch);
            output[ch][i] *= level;  
           }       
      }

  return frames;
}


void CFxClassicOverdrive::reset_params (size_t srate, size_t ch)
{
  AFx::reset_params (srate, ch);
  filter.reset();
}


void CFxClassicOverdrive::dial_level_valueChanged (int value)
{
  if (value == 0)
     {
      level = 1.0f;
      return;
     }

   level = db2lin (value);

   //label->setText (tr ("Volume: %1 dB").arg (value));
}
