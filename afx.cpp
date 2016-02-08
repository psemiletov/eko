#include "afx.h"


AFx::AFx()
{
  bypass = false;
  realtime = true;
  ui_visible = false;
  float_buffer = 0;
 
  state = FXS_STOP;
  classname = "AFx";
  modulename = "AFx";

  samplerate = 1;
  channels = 1;//chann;
  
  wnd_ui = new QWidget();

  vbl_main = new QVBoxLayout;
  wnd_ui->setLayout (vbl_main);

  presets = new CFxPresets;
  

  w_caption = new QWidget; 
  QVBoxLayout *vbl_caption = new QVBoxLayout;
  w_caption->setLayout (vbl_caption);
  
  
  vbl_main->addWidget (presets);
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


void AFx::set_state (FxState s)
{
  state = s;
}


void AFx::reset_params (size_t srate, size_t chann)
{
  samplerate = srate;
  channels = chann;
}

