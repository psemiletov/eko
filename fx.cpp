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


#include "fx.h"
#include "utils.h"
#include "3pass_eq.h"


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
//  qDebug() << "AFx::AFx";

  bypass = false;
  realtime = true;
  ui_visible = false;
  float_buffer = 0;
  wnd_ui = 0;
  state = FXS_STOP;
  name = "AFx";

  samplerate = srate;
  channels = 1;//chann;
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
  list.append (new CFxSimpleEQ (1));
  list.append (new CFxSimpleOverdrive (1));
  list.append (new CFxPitchShift (1));
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
  wnd_ui = new QWidget();

  wnd_ui->setWindowTitle (tr ("Simple Amp"));

  gain = 1.0f;

  QVBoxLayout *v_box = new QVBoxLayout;
  wnd_ui->setLayout (v_box);

  label = new QLabel (tr ("Volume: 1 dB"));

  QDial *dial_gain = new QDial;
  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (-90, 26);

  dial_gain->setValue (0);

  v_box->addWidget (label);
  v_box->addWidget (dial_gain);
}


void CFxSimpleAmp::dial_gain_valueChanged (int value)
{
  if (value == 0)
     {
      gain = 1.0f;
      return;
     }

   gain = db2lin (value);

   label->setText (tr ("Volume: %1 dB").arg (value));
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


CFxSimpleOverdrive::CFxSimpleOverdrive (size_t srate): AFx (srate)
{
  name = "FxSimpleOverdrive";
  wnd_ui = new QWidget();

  wnd_ui->setWindowTitle (tr ("Simple Overdrive"));

  gain = 1.0f;

  QVBoxLayout *v_box = new QVBoxLayout;
  wnd_ui->setLayout (v_box);

  QLabel *l = new QLabel (tr ("Gain"));
  QDial *dial_gain = new QDial;
  dial_gain->setWrapping (false);
  connect (dial_gain, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_gain->setRange (1, 600);

  dial_gain->setValue (1);

  v_box->addWidget (l);
  v_box->addWidget (dial_gain);
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
  wnd_ui = new QWidget();

  wnd_ui->setWindowTitle (tr ("Simple Pitchshifter"));

  ratio = 1.0;

  QVBoxLayout *v_box = new QVBoxLayout;
  wnd_ui->setLayout (v_box);

  QLabel *label = new QLabel (tr ("Ratio: 1.0"));

  QDoubleSpinBox *spb_ratio = new QDoubleSpinBox;
  spb_ratio->setRange (-12, 12);
  spb_ratio->setSingleStep (0.1);
  spb_ratio->setValue (1.0);
  connect (spb_ratio, SIGNAL(valueChanged (double )), this, SLOT(spb_ratio_changed (double )));

  v_box->addWidget (label);
  v_box->addWidget (spb_ratio);
}


void CFxPitchShift::spb_ratio_changed (double value)
{
  ratio = value;
}


CFxPitchShift::~CFxPitchShift()
{
}


AFx* CFxPitchShift::self_create (size_t srate)
{
  return new CFxPitchShift (srate);
}

//ПЕРЕПИСАТЬ СО СТАТИЧНЫМ БУФЕРОМ
size_t CFxPitchShift::execute (float **input, float **output, size_t frames)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       SRC_DATA data;

       data.src_ratio = ratio;

       data.input_frames = frames;
       data.output_frames = frames;

       float *data_in = new float [frames];
       memcpy (data_in, input[ch], frames * sizeof (float));

       data.data_in = data_in;
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
           delete data_in;
           return 0;
          }

       delete [] data_in;
      }
  
  return frames;
}


void AFx::reset_params (size_t srate, size_t chann)
{
  samplerate = srate;
  channels = chann;
}
