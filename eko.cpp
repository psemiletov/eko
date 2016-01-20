/***************************************************************************
 *   2010-2015 by Peter Semiletov                                          *
 *   tea@list.ru                                                           *

started at 25 July 2010
 *                                                                         *
 *   This program is Public Domain                                       *
 ***************************************************************************/


#include <iostream>

#include <samplerate.h>
#include <sndfile.h>
#include <portaudio.h>
#include <limits>
#include <cstdlib>
#include <ctime>

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLibraryInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QStyleFactory>
#include <QClipboard>
#include <QPainter>
#include <QInputDialog>
#include <QMimeData>

#include <QProxyStyle>
#include <QProcess>


#include "eko.h"
#include "utils.h"
#include "gui_utils.h"
#include "logmemo.h"
#include "libretta_interpolator.h"
#include "noisegen.h"
#include "floatbuffer.h"
#include "fxrack.h"

#include "db.h"

#define FM_ENTRY_MODE_NONE 0
#define FM_ENTRY_MODE_OPEN 1
#define FM_ENTRY_MODE_SAVE 2


#define C_METER_CPS 32;


bool b_altmenu;
bool b_monitor_input;

class MyProxyStyle: public QProxyStyle
{
public:
     int styleHint (StyleHint hint, const QStyleOption *option = 0,
                   const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const 
                   {
         
                    if (hint == QStyle::SH_ItemView_ActivateItemOnSingleClick)
                       return 0;
         
                    if (! b_altmenu && hint == QStyle::SH_MenuBar_AltKeyNavigation)
                       return 0;
                  
                    return QProxyStyle::styleHint (hint, option, widget, returnData);
                    }
     
   MyProxyStyle (QStyle *style = 0);
     
};


extern QHash <QString, QString> global_palette;
extern QSettings *settings;
extern int meter_cps;
extern int meter_msecs_delay;

extern int buffer_size_frames;

extern int proxy_video_decoder;

CFloatBuffer *fb_record;


extern CFloatBuffer *sound_clipboard;
extern CFileFormats *file_formats;

extern QString temp_mp3_fname;

extern bool play_r;
extern bool play_l;
extern bool bypass_mixer;

QString fname_tempfile;


SNDFILE *file_temp_handle;


int mono_recording_mode;

int rec_channels;

int pa_device_id_in;
int pa_device_id_out;

extern CDSP *dsp;


QStringList lpa_apis;
QStringList lpa_devices;

PaStream *pa_stream;
PaStream *pa_stream_in;


extern CDocumentHolder *documents;

CLogMemo *glog;


extern int transport_state;
extern double ogg_q;

extern CFxRackWindow *wnd_fxrack;


int simple_counter = 0;


/*
void pa_info()
{
  PaHostApiIndex phai = Pa_GetHostApiCount();
   	
  for (PaHostApiIndex i = 0; i < phai; i++)
      {   
       const PaHostApiInfo *phainfo = Pa_GetHostApiInfo (i); 	       
       lpa_apis.append (phainfo->name);
     }  
  
  
  PaDeviceIndex devindex = Pa_GetDefaultOutputDevice(); 	
   
  devindex = Pa_GetDeviceCount(); 	
  
  for (PaDeviceIndex i = 0; i < devindex - 1; i++)
      {   
       
       PaDeviceInfo *di = Pa_GetDeviceInfo 	(i);
       
       //qDebug() << i << " --- name: " << di->name;
       //qDebug() << i << " --- defaultSampleRate: " << di->defaultSampleRate;
       //qDebug() << i << " --- defaultLowOutputLatency: " << di->defaultLowOutputLatency;
       //qDebug() << i << " --- defaultHighOutputLatency: " << di->defaultHighOutputLatency;
       
       lpa_devices.append (di->name);
     }  
}
*/

/*
static void StreamFinished (void* userData )
{
   paTestData *data = (paTestData *) userData;
   printf( "Stream Completed: %s\n", data->message );
}
*/

//ПЕРЕПИСАТЬ СFloatBuffer
int pa_input_stream_callback (const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
  if (transport_state == STATE_EXIT)
     return paAbort;

  float **p_in = (float **) input;
  float **p_out = (float **) output;

  if (b_monitor_input)
     {
      memcpy (p_out[0], p_in[0],  frameCount * sizeof (float));
      memcpy (p_out[1], p_in[1],  frameCount * sizeof (float));
     } 
  
 if (rec_channels == 2)
    {
     fb_record->settozero();
     
     memcpy (fb_record->buffer[0], p_in[0], frameCount * sizeof (float));
     memcpy (fb_record->buffer[1], p_in[1], frameCount * sizeof (float));
     
     fb_record->fill_interleaved();
     
     sf_writef_float (file_temp_handle, (float *)fb_record->buffer_interleaved, frameCount);
     return paContinue; 	
    }
  else  
  //если пишем моно, что делать с каналами?
      {       
       if (mono_recording_mode == 0)
          sf_writef_float (file_temp_handle, p_in[0], frameCount);
       else 
           sf_writef_float (file_temp_handle, p_in[1], frameCount);
      }     
   
  dsp->process_rec (p_in, 2, frameCount);
  
  return paContinue; 	
}


int pa_stream_callback (const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
  if (transport_state == STATE_EXIT)
     return paAbort;

  CDocument *d = documents->current; 
    
  if (! d)
     {
      transport_state = STATE_STOP;
      return paAbort;
     } 

  float** pchannels = (float **)output;

/*
  if (d->wave_edit->waveform->play_looped)
     {
      if (! d->wave_edit->waveform->selected)
         {
          if (d->wave_edit->waveform->fb->offset >= 
              d->wave_edit->waveform->fb->length_frames - frameCount) 
              {
               d->wave_edit->waveform->fb->offset = 0;
              }                                          
          }
      else //looped, but selected
           {
            if (d->wave_edit->waveform->fb->offset >= d->wave_edit->waveform->frames_end()) 
                d->wave_edit->waveform->fb->offset = d->wave_edit->waveform->frames_start();
           }
     }
   else //not looped at all*/
   
   if (! d->wave_edit->waveform->play_looped)
   if (d->wave_edit->waveform->fb->offset + frameCount >= 
       d->wave_edit->waveform->fb->length_frames)
       {
        qDebug() << "FULL STOP";
       
        d->wave_edit->waveform->timer.stop();
        wnd_fxrack->tm_level_meter.stop();

        transport_state = STATE_STOP;
        wnd_fxrack->fx_rack->set_state_all (FXS_STOP);

        d->wave_edit->waveform->fb->offset = 0;
        d->wave_edit->waveform->scrollbar->setValue (0);
        
        return paAbort;
       }
 
 
  size_t ret = dsp->process (d, frameCount);
  if (ret == 0/* && ! d->wave_edit->waveform->play_looped*/)
     {
      qDebug() << "return from process()";
      transport_state = STATE_STOP;
      
      d->wave_edit->waveform->timer.stop();
      wnd_fxrack->tm_level_meter.stop();

      transport_state = STATE_STOP;
      wnd_fxrack->fx_rack->set_state_all (FXS_STOP);

      d->wave_edit->waveform->fb->offset = 0;
      d->wave_edit->waveform->scrollbar->setValue (0);
      return paAbort;
     }
   
   
  if (play_l)  
      memcpy (pchannels[0], dsp->temp_float_buffer->buffer[0], 
              frameCount * sizeof (float));
  else         
      memset (pchannels[0], 0, frameCount * sizeof (float));

  if (play_r)  
     {
      if (dsp->temp_float_buffer->channels == 2)
         memcpy (pchannels[1], dsp->temp_float_buffer->buffer[1], frameCount * sizeof (float));
      }      
  else
      memset (pchannels[1], 0, frameCount * sizeof (float));
   
  
  return paContinue; 	
}


QStringList get_sound_devices()
{
  QStringList l;
  PaDeviceIndex devindex = Pa_GetDeviceCount() - 1; 	
  
  for (PaDeviceIndex i = 0; i < devindex; i++)
      {   
       const PaDeviceInfo *di = Pa_GetDeviceInfo (i);
       
      /* qDebug() << "str_from_locale (di->name): " << str_from_locale (di->name);
       qDebug() << "in: " << di->maxInputChannels;
       qDebug() << "out: " << di->maxOutputChannels;
       */
       #if defined(Q_OS_WIN) || defined(Q_OS_OS2)
       
       l.append (str_from_locale (di->name));
       
       #else

       l.append (di->name);
       
       #endif
     }  
  
  return l;
}


void CEKO::init_styles()
{
#if QT_VERSION >= 0x050000

  QString default_style = qApp->style()->objectName();

  if (default_style == "GTK+") //can be buggy
     default_style = "Fusion";

#else

  QString default_style = qApp->style()->objectName();

  if (default_style == "GTK+") //can be buggy
     default_style = "Cleanlooks";

#endif

  fname_stylesheet = settings->value ("fname_stylesheet", ":/themes/TEA").toString();
   
  MyProxyStyle *ps = new MyProxyStyle (QStyleFactory::create (settings->value ("ui_style", default_style).toString()));
  
  QApplication::setStyle (ps);

  update_stylesheet (fname_stylesheet);
}


void pa_init()
{
  PaError err = Pa_Initialize();
  qDebug() << Pa_GetErrorText (err);
  pa_stream = 0;
  pa_stream_in = 0;
}


void pa_done()
{
  if (pa_stream)
     {
      Pa_AbortStream (pa_stream);
      Pa_CloseStream (pa_stream);
     } 

  if (pa_stream_in)
     {
      Pa_AbortStream (pa_stream_in);
      Pa_CloseStream (pa_stream_in);
     } 

  Pa_Terminate(); 	
}


void CEKO::create_paths()
{
  QDir dr;
  dir_config = dr.homePath();

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)

  dir_config.append ("/eko");

#else

  dir_config.append ("/.config/eko");

#endif

  dr.setPath (dir_config);
  if (! dr.exists())
     dr.mkpath (dir_config);

  fname_fif.append (dir_config).append ("/fif");
  fname_places_bookmarks.append (dir_config).append ("/places_bookmarks");
  fname_tempfile = "/eko-temp-777.wav";
  fname_tempparamfile.append (QDir::tempPath()).append ("/ekoparam.tmp");

  dir_profiles.append (dir_config).append ("/profiles");
  
  dr.setPath (dir_profiles);
  if (! dr.exists())
     dr.mkpath (dir_profiles);

  dir_sessions.append (dir_config).append ("/sessions");

  dr.setPath (dir_sessions);
  if (! dr.exists())
     dr.mkpath (dir_sessions);

  dir_palettes.append (dir_config).append ("/palettes");

  dr.setPath (dir_palettes);
  if (! dr.exists())
     dr.mkpath (dir_palettes);
     
     
  dir_themes.append (dir_config).append ("/themes");
  dr.setPath (dir_themes);
  if (! dr.exists())
     dr.mkpath (dir_themes);
   
}


void CEKO::readSettings()
{
  b_altmenu = settings->value ("b_altmenu", "0").toBool(); 

  b_monitor_input = settings->value ("b_monitor_input", 0).toBool();

  proxy_video_decoder = settings->value ("proxy_video_decoder", "0").toInt(); 

  fname_def_palette = settings->value ("fname_def_palette", ":/palettes/EKO").toString();
  
  ogg_q = settings->value ("ogg_q", 0.5d).toDouble();

  QPoint pos = settings->value ("pos", QPoint (1, 1)).toPoint();
  QSize size = settings->value ("size", QSize (800, 600)).toSize();
  mainSplitter->restoreState (settings->value ("splitterSizes").toByteArray());
  resize (size);
  move (pos);

  buffer_size_frames = settings->value ("buffer_size_frames", 2048).toInt();
  
  mono_recording_mode = settings->value ("mono_recording_mode", 0).toInt();
}


void CEKO::writeSettings()
{
  settings->setValue ("pos", pos());
  settings->setValue ("size", size());
  settings->setValue ("splitterSizes", mainSplitter->saveState());
  settings->setValue ("spl_fman", spl_fman->saveState());
  settings->setValue ("icons_size", iconSize().width());
  settings->setValue ("dir_last", dir_last);
  settings->setValue ("fname_def_palette", fname_def_palette);
  settings->setValue ("VERSION_NUMBER", QString (VERSION_NUMBER));
  settings->setValue ("state", saveState());

  if (wnd_fxrack->isVisible())
     {
      settings->setValue ("wnd_fxrack.pos", wnd_fxrack->pos());
      settings->setValue ("wnd_fxrack.size", wnd_fxrack->size());
     }  

  delete settings;
}


void CEKO::create_main_widget()
{
  QWidget *main_widget = new QWidget;
  QVBoxLayout *v_box = new QVBoxLayout;
  main_widget->setLayout (v_box);

  main_tab_widget = new QTabWidget;
  
  main_tab_widget->setTabPosition (QTabWidget::East);
 
  main_tab_widget->setTabShape (QTabWidget::Triangular);

  tab_widget = new QTabWidget;
  
#if QT_VERSION >= 0x040500
  tab_widget->setMovable (true);
#endif
  
  QPushButton *bt_close = new QPushButton ("X", this);
  connect (bt_close, SIGNAL(clicked()), this, SLOT(close_current()));
  tab_widget->setCornerWidget (bt_close);
  
  log_memo = new QPlainTextEdit;
  log = new CLogMemo (log_memo);
  
  glog = log;
  
  mainSplitter = new QSplitter (Qt::Vertical);
  v_box->addWidget (mainSplitter);

  cmb_fif = new QComboBox;
  cmb_fif->setInsertPolicy (QComboBox::InsertAtTop); 

  cmb_fif->setEditable (true);
  fif = cmb_fif->lineEdit();
  connect (fif, SIGNAL(returnPressed()), this, SLOT(search_find()));

  main_tab_widget->setMinimumHeight (10);
  log_memo->setMinimumHeight (10);

  mainSplitter->addWidget (main_tab_widget);
  mainSplitter->addWidget (log_memo);

  QHBoxLayout *lt_fte = new QHBoxLayout;
  v_box->addLayout (lt_fte);

  QToolButton *bt_find = new QToolButton (this);
  QToolButton *bt_prev = new QToolButton (this);
  QToolButton *bt_next = new QToolButton (this);

  bt_next->setArrowType (Qt::RightArrow);
  bt_prev->setArrowType (Qt::LeftArrow);

  connect (bt_find, SIGNAL(clicked()), this, SLOT(search_find()));
  connect (bt_next, SIGNAL(clicked()), this, SLOT(search_find_next()));
  connect (bt_prev, SIGNAL(clicked()), this, SLOT(search_find_prev()));

  bt_find->setIcon (QIcon (":/icons/search_find.png"));

  QLabel *l_fif = new QLabel (tr ("FIF"));
 
  lt_fte->addWidget (l_fif, 0, Qt::AlignRight);
  lt_fte->addWidget (cmb_fif, 1);
  lt_fte->addWidget (bt_find);
  lt_fte->addWidget (bt_prev);
  lt_fte->addWidget (bt_next);

  mainSplitter->setStretchFactor (1, 1);

  idx_tab_edit = main_tab_widget->addTab (tab_widget, tr ("edit"));
  setCentralWidget (main_widget);

  connect (tab_widget, SIGNAL(currentChanged(int)), this, SLOT(pageChanged(int)));
}


CEKO::CEKO()
{
  init_db();
  
  srand (static_cast <unsigned> (time(0)));

  pa_init();

  play_r = true;
  play_l = true;
  
  b_monitor_input = false;
  
  bypass_mixer = false;    
  
  temp_mp3_fname = QDir::tempPath() + "/eko-temp-from-mp3.wav";
     
  file_temp_handle = 0;
  

  meter_cps = C_METER_CPS;
  meter_msecs_delay = 1000 / meter_cps;

//  qDebug() << "meter_msecs_delay = " << meter_msecs_delay;

  ui_update = true;

  file_formats_init();

  lv_menuitems = NULL;
  fm_entry_mode = FM_ENTRY_MODE_NONE;
  
  idx_tab_edit = 0;
  idx_tab_tune = 0;
  idx_tab_fman = 0;
  idx_tab_learn = 0;
  
  transport_state = STATE_STOP;
  
  transport_control = new CTransportControl;
  connect(transport_control, SIGNAL(play_pause()), this, SLOT(slot_transport_play()));
  connect(transport_control, SIGNAL(stop()), this, SLOT(slot_transport_stop()));
  
  create_paths();
  
  QString sfilename = dir_config + "/eko.conf";
  settings = new QSettings (sfilename, QSettings::IniFormat);

  if (settings->value ("override_locale", 0).toBool())
     {
      QString ts = settings->value ("override_locale_val", "en").toString();
      if (ts.length() != 2)
         ts = "en";
      
      qtTranslator.load (QString ("qt_%1").arg (ts),
                         QLibraryInfo::location (QLibraryInfo::TranslationsPath));
      qApp->installTranslator (&qtTranslator);

      myappTranslator.load (":/translations/eko_" + ts);
      qApp->installTranslator (&myappTranslator);
     }
  else
      {
       qtTranslator.load (QString ("qt_%1").arg (QLocale::system().name()),
                          QLibraryInfo::location (QLibraryInfo::TranslationsPath));
       qApp->installTranslator (&qtTranslator);

       myappTranslator.load (":/translations/eko_" + QLocale::system().name());
       qApp->installTranslator (&myappTranslator);
      }

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  init_styles();
  
  update_sessions();
  update_palettes();
  update_themes();
  update_profiles();

  setMinimumSize (12, 12);
  
  create_main_widget();

  idx_prev = 0;
  connect (main_tab_widget, SIGNAL(currentChanged(int)), this, SLOT(main_tab_page_changed(int)));

  readSettings();
  
  documents = new CDocumentHolder;
  documents->parent_wnd = this;
  documents->tab_widget = tab_widget;
  documents->recent_menu = menu_file_recent;
  documents->recent_list_fname.append (dir_config).append ("/eko_recent");
  documents->reload_recent_list();
  documents->update_recent_menu();
  documents->log = log;
  documents->status_bar = statusBar();
  documents->dir_config = dir_config;
  documents->load_palette (fname_def_palette);

  load_palette (fname_def_palette);

  
  shortcuts = new CShortcuts (this);
  shortcuts->fname.append (dir_config).append ("/shortcuts");
  shortcuts->load_from_file (shortcuts->fname);

  sl_fif_history = qstring_load (fname_fif).split ("\n");
  cmb_fif->addItems (sl_fif_history);
  cmb_fif->clearEditText(); 

  createFman();
  createOptions();
  createManual();
 
//  updateFonts();

  dir_last = settings->value ("dir_last", QDir::homePath()).toString();
  b_preview = settings->value ("b_preview", false).toBool(); 
  
  l_status = new QLabel;
  pb_status = new QProgressBar;
  pb_status->setRange (0, 0);

  statusBar()->insertPermanentWidget (0, pb_status);
  statusBar()->insertPermanentWidget (1, l_status);

  pb_status->hide();
  documents->l_status_bar = l_status;
  documents->l_maintime = l_maintime;

  restoreState (settings->value ("state", QByteArray()).toByteArray());

  text_file_browser = new QTextBrowser;
  text_file_browser->setWindowTitle (tr ("text browser"));
  text_file_browser->setReadOnly (true);
  text_file_browser->setOpenExternalLinks (true); 

  QString vn = settings->value ("VERSION_NUMBER", "").toString();
  if (vn.isEmpty() || vn != QString (VERSION_NUMBER))
      help_show_news();

  if (settings->value ("session_restore", false).toBool())
     {
      QString fname_session (dir_sessions);
      fname_session.append ("/def-session-777");
      documents->load_from_session (fname_session);
     }
  
  ui_update = false;
  
  setAcceptDrops (true);
  qApp->setWindowIcon (QIcon (":/icons/eko_icon.png"));
  log->log (tr ("<b>EKO %1 @ http://semiletov.org/eko</b><br>or <i>https://github.com/psemiletov/eko</i><br>by Peter Semiletov (tea@list.ru)<br>read the Manual under the <i>learn</i> tab!").arg (QString (VERSION_NUMBER)));
  
  idx_tab_edit_activate();
      
  fb_record = new CFloatBuffer (65536, 2);     
      
  dsp = new CDSP;
      
  wnd_fxrack = new CFxRackWindow;
  
  connect(wnd_fxrack->bt_apply, SIGNAL (clicked()),this, SLOT (apply_fx_clicked()));
  
   
  documents->transport_control = transport_control;
  
  QPoint ps = settings->value ("wnd_fxrack.pos", QPoint (pos().x() + width() + 3, y())).toPoint();
  
  if (settings->value ("wnd_fxrack_visible", true).toBool())
     {
      wnd_fxrack->move (ps);
      wnd_fxrack->show();
      wnd_fxrack->raise();
     } 
     
  
  if (file_exists (settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile))
     QFile::remove (settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile);
  
  
  handle_args();
}


void CEKO::leaving_tune()
{
  ogg_q = spb_ogg_q->value();
  settings->setValue ("ogg_q", ogg_q);
  
  settings->setValue ("mp3_encode", ed_mp3_encode->text());
}


void CEKO::closeEvent (QCloseEvent *event)
{
  transport_state = STATE_EXIT;

  pa_done();

  QString tfn = settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile;
     
  if (file_exists (tfn))
      QFile::remove (tfn );
  

  if (main_tab_widget->currentIndex() == idx_tab_tune)
     leaving_tune();

  if (settings->value("session_restore", false).toBool())
     {
      QString fname_session (dir_sessions);
      fname_session.append ("/def-session-777");
      documents->save_to_session (fname_session);
     }

  writeSettings();

  qstring_save (fname_fif, sl_fif_history.join ("\n"));
  
  delete shortcuts;
  
  delete dsp;
  dsp = 0;
  
  delete wnd_fxrack;
  wnd_fxrack = 0;
  
  delete documents;
  documents = 0;
  
  file_formats_done();

  event->accept();
}


void CEKO::newFile()
{
  transport_state = STATE_STOP;
  
  if (pa_stream)
     {
       Pa_CloseStream (pa_stream);	
       pa_stream = 0;
     }

  if (pa_stream_in)
     {
       Pa_AbortStream (pa_stream_in);	
       pa_stream_in = 0;
     }

  CDocument *new_document = documents->create_new();
  
  new_document->wave_edit->waveform->recalc_view();
  new_document->wave_edit->waveform->prepare_image();
  new_document->wave_edit->waveform->init_state = false;
  new_document->wave_edit->timeruler->init_state = false;
  new_document->wave_edit->waveform->update();
  
  main_tab_widget->setCurrentIndex (idx_tab_edit);
}


void CEKO::open()
{
 // qDebug() << "CEKO::open() - start";

  transport_state = STATE_STOP;
  wnd_fxrack->fx_rack->set_state_all (FXS_STOP);

  if (pa_stream)
     {
      Pa_AbortStream (pa_stream);	
      pa_stream = 0;
     }

  if (pa_stream_in)
     {
       Pa_AbortStream (pa_stream_in);	
       pa_stream_in = 0;
     }


  if (! settings->value ("use_trad_dialogs", "0").toBool())
     {
      CDocument *d = documents->get_current();

      if (d)
         {
          if (file_exists (d->file_name))
              fman->nav (get_file_path (d->file_name));
          else
              fman->nav (dir_last);
         }
      else
          fman->nav (dir_last);

      main_tab_widget->setCurrentIndex (idx_tab_fman);
      fm_entry_mode = FM_ENTRY_MODE_OPEN;

      return;
     }

  QFileDialog dialog (this);
  QSize size = settings->value ("dialog_size", QSize (width(), height())).toSize();
  dialog.resize (size);

  dialog.setFilter (QDir::AllEntries | QDir::Hidden);

  QList<QUrl> sidebarUrls = dialog.sidebarUrls();
  QList<QUrl> sidebarUrls_old = dialog.sidebarUrls();

  sidebarUrls.append (QUrl::fromLocalFile (dir_sessions));

#ifdef Q_QS_X11

  QDir volDir("/mnt");
  QStringList volumes (volDir.entryList (volDir.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes)  
          sidebarUrls.append (QUrl::fromLocalFile ("/mnt/" + v));

  QDir volDir2 ("/media");
  QStringList volumes2 (volDir2.entryList (volDir2.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes2)
          sidebarUrls.append (QUrl::fromLocalFile ("/media/" + v));

  
#endif

  dialog.setSidebarUrls (sidebarUrls);

  dialog.setFileMode (QFileDialog::ExistingFiles);
  dialog.setAcceptMode (QFileDialog::AcceptOpen);

  CDocument *d = documents->get_current();

  if (d)
     {
      if (file_exists (d->file_name))
          dialog.setDirectory (get_file_path (d->file_name));
      else
          dialog.setDirectory (dir_last);
     }
  else
      dialog.setDirectory (dir_last);


  dialog.setNameFilter (tr ("All (*);;WAV files (*.wav);;Compressed files (*.ogg *.flac *.mp3)"));

  QStringList fileNames;
  
  if (dialog.exec())
     {
      dialog.setSidebarUrls (sidebarUrls_old);

      fileNames = dialog.selectedFiles();
      
      foreach (QString fn, fileNames)
              {
               CDocument *d = documents->open_file (fn);
               if (d)
                   dir_last = get_file_path (d->file_name);
              }
     }
  else
      dialog.setSidebarUrls (sidebarUrls_old);


  transport_state = STATE_STOP;

  settings->setValue ("dialog_size", dialog.size());
  update_dyn_menus();

  //qDebug() << "CEKO::open() - end";
}


bool CEKO::save()
{
  CDocument *d = documents->get_current();

  if (! d)
     return false;

  if (d->wave_edit->isReadOnly())
     {
      log->log (tr ("This file is open in the read-only mode. You can save it with another name using <b>Save as</b>"));
      return false;
     }

  if (file_exists (d->file_name))
     d->save_with_name (d->file_name);
  else
      return saveAs();

  return true;
}


bool CEKO::saveAs()
{
  CDocument *d = documents->get_current();
  
  if (! d)
     return false;

  if (! settings->value ("use_trad_dialogs", "0").toBool())
     {
      main_tab_widget->setCurrentIndex (idx_tab_fman);
      fm_entry_mode = FM_ENTRY_MODE_SAVE;

      if (file_exists (d->file_name))
         fman->nav (get_file_path (d->file_name));
      else
          fman->nav (dir_last);

      ed_fman_fname->setFocus();   
          
      return true;
     }

  QFileDialog dialog (this);
  QSize size = settings->value ("dialog_size", QSize (width(), height())).toSize();
  dialog.resize (size);

  dialog.setFilter(QDir::AllEntries | QDir::Hidden);
  dialog.setOption (QFileDialog::DontUseNativeDialog, true);


  QList<QUrl> sidebarUrls = dialog.sidebarUrls();
  QList<QUrl> sidebarUrls_old = dialog.sidebarUrls();

  sidebarUrls.append(QUrl::fromLocalFile(dir_sessions));

#ifdef Q_QS_X11

  QDir volDir("/mnt");
  QStringList volumes (volDir.entryList (volDir.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes)
          sidebarUrls.append (QUrl::fromLocalFile ("/mnt/" + v));

  QDir volDir2 ("/media");
  QStringList volumes2 (volDir2.entryList (volDir2.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes2)
          sidebarUrls.append (QUrl::fromLocalFile ("/media/" + v));

#endif

  dialog.setSidebarUrls(sidebarUrls);

  dialog.setFileMode (QFileDialog::AnyFile);
  dialog.setAcceptMode (QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite (false);
  dialog.setDirectory (dir_last);

  if (dialog.exec())
     {
      dialog.setSidebarUrls (sidebarUrls_old);

      QString fileName = dialog.selectedFiles().at(0);

      if (file_exists (fileName))
         {
          int ret = QMessageBox::warning (this, "EKO",
                                          tr ("%1 already exists\n"
                                          "Do you want to overwrite?")
                                           .arg (fileName),
                                          QMessageBox::Yes | QMessageBox::Default,
                                          QMessageBox::Cancel | QMessageBox::Escape);

          if (ret == QMessageBox::Cancel)
             return false;
         }

      d->save_with_name (fileName);
      
      update_dyn_menus();

      QFileInfo f (d->file_name);
      dir_last = f.path();
     }
   else
       dialog.setSidebarUrls (sidebarUrls_old);

  settings->setValue ("dialog_size", dialog.size());
  return true;
}


void CEKO::about()
{
  CAboutWindow *a = new CAboutWindow();
  a->move (x() + 20, y() + 20);
  a->show();
}


void CEKO::createActions()
{
  act_test = new QAction (QIcon (":/icons/file-save.png"), tr ("Test"), this);
  connect (act_test, SIGNAL(triggered()), this, SLOT(test()));

  newAct = new QAction (QIcon (":/icons/file-new.png"), tr ("New"), this);
  newAct->setShortcut (QKeySequence ("Ctrl+N"));
  newAct->setStatusTip (tr ("Create a new file"));
  connect (newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  QIcon ic_file_open (":/icons/file-open.png");
  ic_file_open.addFile (":/icons/file-open-active.png", QSize(), QIcon::Active); 

  openAct = new QAction (ic_file_open, tr ("Open file"), this);
  openAct->setStatusTip (tr ("Open an existing file"));
  connect (openAct, SIGNAL(triggered()), this, SLOT(open()));


  QIcon ic_file_save (":/icons/file-save.png");
  ic_file_save.addFile (":/icons/file-save-active.png", QSize(), QIcon::Active); 
  
  saveAct = new QAction (ic_file_save, tr ("Save"), this);
  saveAct->setShortcut (QKeySequence ("Ctrl+S"));
  saveAct->setStatusTip (tr ("Save the document to disk"));
  connect (saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction (QIcon (":/icons/file-save-as.png"), tr ("Save As"), this);
  saveAsAct->setStatusTip (tr ("Save the document under a new name"));
  connect (saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  exitAct = new QAction (tr ("Exit"), this);
  exitAct->setShortcut (QKeySequence ("Ctrl+Q"));
  exitAct->setStatusTip (tr ("Exit the application"));
  connect (exitAct, SIGNAL(triggered()), this, SLOT(close()));

  QIcon ic_edit_cut (":/icons/edit-cut.png");
  ic_edit_cut.addFile (":/icons/edit-cut-active.png", QSize(), QIcon::Active); 

  cutAct = new QAction (ic_edit_cut, tr ("Cut"), this);
  cutAct->setShortcut (QKeySequence ("Ctrl+X"));
  cutAct->setStatusTip (tr ("Cut the current selection's contents to the clipboard"));
  connect (cutAct, SIGNAL(triggered()), this, SLOT(ed_cut()));

  QIcon ic_edit_copy (":/icons/edit-copy.png");
  ic_edit_copy.addFile (":/icons/edit-copy-active.png", QSize(), QIcon::Active); 
  
  copyAct = new QAction (ic_edit_copy, tr("Copy"), this);
  copyAct->setShortcut (QKeySequence ("Ctrl+C"));
  copyAct->setStatusTip (tr ("Copy the current selection's contents to the clipboard"));
  connect (copyAct, SIGNAL(triggered()), this, SLOT(ed_copy()));


  QIcon ic_edit_paste (":/icons/edit-paste.png");
  ic_edit_paste.addFile (":/icons/edit-paste-active.png", QSize(), QIcon::Active); 

  pasteAct = new QAction (ic_edit_paste, tr("Paste"), this);
  pasteAct->setShortcut (QKeySequence ("Ctrl+V"));
  pasteAct->setStatusTip (tr ("Paste the clipboard's contents into the current selection"));
  connect (pasteAct, SIGNAL(triggered()), this, SLOT(ed_paste()));

  undoAct = new QAction (tr ("Undo"), this);
  undoAct->setShortcut (QKeySequence ("Ctrl+Z"));
  connect (undoAct, SIGNAL(triggered()), this, SLOT(ed_undo()));

  redoAct = new QAction (tr ("Redo"), this);
  connect (redoAct, SIGNAL(triggered()), this, SLOT(ed_redo()));

  aboutAct = new QAction (tr ("About"), this);
  connect (aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction (tr ("About Qt"), this);
  connect (aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    
  transport_play = new QAction (QIcon (":/icons/play.png"), tr ("Play/Pause (Space)"), this);
  transport_play->setStatusTip (tr ("Play/Pause"));
  connect (transport_play, SIGNAL(triggered()), this, SLOT(slot_transport_play()));

  transport_stop = new QAction (QIcon (":/icons/stop.png"), tr ("Stop"), this);
  transport_stop->setStatusTip (tr ("Stop"));
  connect (transport_stop, SIGNAL(triggered()), this, SLOT(slot_transport_stop()));
}


void CEKO::createMenus()
{
  fileMenu = menuBar()->addMenu (tr ("File"));
  fileMenu->setTearOffEnabled (true);

  //fileMenu->addAction (act_test);

  fileMenu->addAction (newAct);
  
  add_to_menu (fileMenu, tr ("Record"), SLOT(file_record()));
  add_to_menu (fileMenu, tr ("Open"), SLOT(open()), "Ctrl+O", ":/icons/file-open.png");
  add_to_menu (fileMenu, tr ("Last closed file"), SLOT(file_last_opened()));
  
  fileMenu->addSeparator();

  add_to_menu (fileMenu, tr ("Change format"), SLOT(file_change_format()));
  add_to_menu (fileMenu, tr ("File info"), SLOT(file_info()));

  fileMenu->addSeparator();

  fileMenu->addAction (saveAct);
  fileMenu->addAction (saveAsAct);

  QMenu *tm = fileMenu->addMenu (tr ("Save as different"));
  tm->setTearOffEnabled (true);

  add_to_menu (tm, tr ("Save timestamped version"), SLOT(file_save_version()), "Alt+S");
  add_to_menu (tm, tr ("Save session"), SLOT(session_save_as()));

  fileMenu->addSeparator();

  tm = fileMenu->addMenu (tr ("Import/Export"));
  tm->setTearOffEnabled (true);
  add_to_menu (tm, tr ("Import from RAW"), SLOT(file_import_raw()));
  add_to_menu (tm, tr ("Export to MP3"), SLOT(file_export_mp3()));

 // add_to_menu (tm, tr ("Export to RAW"), SLOT(file_export_raw()));

  fileMenu->addSeparator();

  menu_file_actions = fileMenu->addMenu (tr ("File actions"));
  add_to_menu (menu_file_actions, tr ("Reload"), SLOT(file_reload()));
      
  menu_file_recent = fileMenu->addMenu (tr ("Recent files"));
  menu_file_sessions = fileMenu->addMenu (tr ("Sessions"));

  fileMenu->addSeparator();

  add_to_menu (fileMenu, tr ("Close current"), SLOT(close_current()), "Ctrl+W");

  fileMenu->addAction (exitAct);


  editMenu = menuBar()->addMenu (tr ("Edit"));
  editMenu->setTearOffEnabled (true);

  editMenu->addAction (cutAct);
  editMenu->addAction (copyAct);
  editMenu->addAction (pasteAct);

  add_to_menu (editMenu, tr ("Copy to new"), SLOT(ed_copy_to_new()));
  add_to_menu (editMenu, tr ("Copy to new (default format)"), SLOT(ed_copy_to_new_fmt()));

  add_to_menu (editMenu, tr ("Delete (Del)"), SLOT(ed_delete()));
  add_to_menu (editMenu, tr ("Trim"), SLOT(ed_trim()));
  
  editMenu->addSeparator();

  add_to_menu (editMenu, tr ("Copy current file name"), SLOT(edit_copy_current_fname()), "Ctrl+W");

  editMenu->addSeparator();

  add_to_menu (editMenu, tr ("Select all"), SLOT(ed_select_all()), "Ctrl+A");
  add_to_menu (editMenu, tr ("Deselect"), SLOT(ed_deselect()));
  
  editMenu->addSeparator();


  editMenu->addAction (undoAct);
  //editMenu->addAction (redoAct);
  
  menu_search = menuBar()->addMenu (tr ("Search"));
  menu_search->setTearOffEnabled (true);

  add_to_menu (menu_search, tr ("Find"), SLOT(search_find()));
  add_to_menu (menu_search, tr ("Find next"), SLOT(search_find_next()),"F3");
  add_to_menu (menu_search, tr ("Find previous"), SLOT(search_find_prev()),"Ctrl+F3");


  menu_functions = menuBar()->addMenu (tr ("Functions"));
  menu_functions->setTearOffEnabled (true);


  tm = menu_functions->addMenu (tr ("Silence"));
  tm->setTearOffEnabled (true);

  add_to_menu (tm, tr ("Insert silence"), SLOT(fn_insert_silence()));
  add_to_menu (tm, tr ("Silence selection"), SLOT(fn_silence_selection()));
  

  tm = menu_functions->addMenu (tr ("Generate"));
  tm->setTearOffEnabled (true);
  
  add_to_menu (tm, tr ("Sine wave"), SLOT(generate_sine()));
  add_to_menu (tm, tr ("Noise"), SLOT(generate_noise()));

  
  menu_fn_channels = menu_functions->addMenu (tr ("Channels"));
  menu_fn_channels->setTearOffEnabled (true);

  add_to_menu (menu_fn_channels, tr ("Stereo to mono"), SLOT(fn_stereo_to_mono()));
  add_to_menu (menu_fn_channels, tr ("Mono to stereo 50%"), SLOT(fn_mono_to_stereo_half()));
  add_to_menu (menu_fn_channels, tr ("Mono to stereo full"), SLOT(fn_mono_to_stereo_full()));
  add_to_menu (menu_fn_channels, tr ("5.1 to stereo"), SLOT(fn_51_to_stereo()));
  add_to_menu (menu_fn_channels, tr ("5.1 to stereo (another algorhythm)"), SLOT(fn_51_to_stereo_dlike()));


  add_to_menu (menu_fn_channels, tr ("Swap channels"), SLOT(fn_swap_channels()));
  add_to_menu (menu_fn_channels, tr ("Copy channel"), SLOT(fn_copy_channel()));
  add_to_menu (menu_fn_channels, tr ("Mute channel"), SLOT(fn_mute_channel()));

  
  tm = menu_functions->addMenu (tr ("Fade"));
  tm->setTearOffEnabled (true);

  add_to_menu (tm, tr ("Fade in"), SLOT(fn_fade_in()));
  add_to_menu (tm, tr ("Fade out"), SLOT(fn_fade_out()));


  tm = menu_functions->addMenu (tr ("Dynamics"));
  tm->setTearOffEnabled (true);

  add_to_menu (tm, tr ("Normalize"), SLOT(fn_norm()));
  add_to_menu (tm, tr ("Apply volume envelope"), SLOT(fn_apply_vol_envelope()));
  add_to_menu (tm, tr ("Remove volume envelope"), SLOT(fn_delete_vol_envelope()));
  
  
  

  //tm = menu_functions->addMenu (tr ("Filter"));
  //tm->setTearOffEnabled (true);

  tm = menu_functions->addMenu (tr ("Analyze"));
  tm->setTearOffEnabled (true);

  add_to_menu (tm, tr ("RMS"), SLOT(fn_stat_rms()));
  add_to_menu (tm, tr ("Levels"), SLOT(fn_detect_average_value()));


  tm = menu_functions->addMenu (tr ("DC Offset"));
  tm->setTearOffEnabled (true);

  add_to_menu (tm, tr ("Detect DC offset"), SLOT(fn_dc_offset_detect()));
  add_to_menu (tm, tr ("Fix DC offset manually"), SLOT(fn_dc_offset_fix_manually()));
  add_to_menu (tm, tr ("Fix DC offset auto"), SLOT(fn_dc_offset_fix_auto()));


  tm = menu_functions->addMenu (tr ("Other"));
  tm->setTearOffEnabled (true);
 
  add_to_menu (tm, tr ("Reverse"), SLOT(fn_reverse()));

  
  menu_nav = menuBar()->addMenu (tr ("Nav"));
  menu_nav->setTearOffEnabled (true);

  menu_nav->addAction (transport_play);
  menu_nav->addAction (transport_stop);

  add_to_menu (menu_nav, tr ("Play looped on/off"), SLOT(nav_play_looped()));

  menu_nav->addSeparator();
  

  //add_to_menu (menu_nav, tr ("Save position"), SLOT(nav_save_pos()));
  //add_to_menu (menu_nav, tr ("Go to saved position"), SLOT(nav_goto_pos()));
  add_to_menu (menu_nav, tr ("Next tab"), SLOT(nav_goto_right_tab()));
  add_to_menu (menu_nav, tr ("Prev tab"), SLOT(nav_goto_left_tab()));
  add_to_menu (menu_nav, tr ("Focus the Famous input field"), SLOT(nav_focus_to_fif()), "Ctrl+F");
  add_to_menu (menu_nav, tr ("Focus the editor"), SLOT(nav_focus_to_editor()));

  menu_fm = menuBar()->addMenu (tr ("Fm"));
  menu_fm->setTearOffEnabled (true);

  menu_fm_file_ops = menu_fm->addMenu (tr ("File operations"));
  menu_fm_file_ops->setTearOffEnabled (true);

  add_to_menu (menu_fm_file_ops, tr ("Create new directory"), SLOT(fman_create_dir()));
  add_to_menu (menu_fm_file_ops, tr ("Rename"), SLOT(fman_rename()));
  add_to_menu (menu_fm_file_ops, tr ("Delete file"), SLOT(fman_delete()));

  menu_fm_file_infos = menu_fm->addMenu (tr ("File information"));
  menu_fm_file_infos->setTearOffEnabled (true);

  add_to_menu (menu_fm_file_infos, tr ("Full info"), SLOT(fm_full_info()));

  
  add_to_menu (menu_fm, tr ("Go to home dir"), SLOT(fman_home()));
  add_to_menu (menu_fm, tr ("Refresh current dir"), SLOT(fman_refresh()));

  add_to_menu (menu_fm, tr ("Select by regexp"), SLOT(fman_select_by_regexp()));
  add_to_menu (menu_fm, tr ("Deselect by regexp"), SLOT(fman_deselect_by_regexp()));
  
  
  menu_view = menuBar()->addMenu (tr ("View"));
  menu_view->setTearOffEnabled (true);

  menu_view_palettes = menu_view->addMenu (tr ("Palettes"));
  menu_view_palettes->setTearOffEnabled (true);

  menu_view_themes = menu_view->addMenu (tr ("Themes"));
  menu_view_themes->setTearOffEnabled (true);



  menu_view_profiles = menu_view->addMenu (tr ("Profiles"));
  menu_view_profiles->setTearOffEnabled (true);

  add_to_menu (menu_view, tr ("Save profile"), SLOT(profile_save_as()));

  add_to_menu (menu_view, tr ("Show/hide FX Rack"), SLOT(view_show_mixer()), "Alt+M");
  
  add_to_menu (menu_view, tr ("Toggle fullscreen"), SLOT(view_toggle_fs()));
  add_to_menu (menu_view, tr ("Stay on top"), SLOT(view_stay_on_top()));


  helpMenu = menuBar()->addMenu (tr ("Help"));
  helpMenu->setTearOffEnabled (true);


  add_to_menu (helpMenu, tr ("System check"), SLOT(help_system_check()));
  
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
  add_to_menu (helpMenu, tr ("NEWS"), SLOT(help_show_news()));
  add_to_menu (helpMenu, "TODO", SLOT(help_show_todo()));
  add_to_menu (helpMenu, "ChangeLog", SLOT(help_show_changelog()));
  add_to_menu (helpMenu, tr ("License"), SLOT(help_show_gpl()));
}


void CEKO::createToolBars()
{
  openAct->setMenu (menu_file_recent);

  fileToolBar = addToolBar (tr ("File"));
  fileToolBar->setObjectName ("fileToolBar");
  fileToolBar->addAction (newAct);
  fileToolBar->addAction (openAct);
  fileToolBar->addAction (saveAct);

  editToolBar = addToolBar (tr ("Edit"));
  editToolBar->setObjectName ("editToolBar");
  editToolBar->addAction (cutAct);
  editToolBar->addAction (copyAct);
  editToolBar->addAction (pasteAct);
  
  transportToolBar = addToolBar (tr ("Transport"));
  transportToolBar->setObjectName ("transportToolBar");
  
  cb_play_looped = new QCheckBox (tr ("looped")); 
  connect(cb_play_looped, SIGNAL(stateChanged (int )), this, SLOT(cb_play_looped_changed (int )));
  transportToolBar->addWidget (cb_play_looped);
    
  transportToolBar->addAction (transport_play);
  transportToolBar->addAction (transport_stop);
    
  QLabel *lt = new QLabel (tr ("Current time: "));
  l_maintime = new QLabel; 

  transportToolBar->addWidget (lt);
  transportToolBar->addWidget (l_maintime);
}


void CEKO::createStatusBar()
{
  statusBar()->showMessage (tr ("Ready"));
}


CEKO::~CEKO()
{
  QFile::remove (temp_mp3_fname);

  delete text_file_browser;
  delete fman;
  delete log;
  delete transport_control;
  delete fb_record;
}


void CEKO::pageChanged (int index)
{
  transport_state = STATE_STOP;
  
  if (pa_stream)
     {
      Pa_CloseStream (pa_stream);	
      pa_stream = 0;
     }

  if (pa_stream_in)
     {
       Pa_CloseStream (pa_stream_in);	
       pa_stream_in = 0;
     }


  if (index == -1)
     {
      documents->current = 0;
      return;  
     }
  
  documents->current = documents->list[index];
  
  if (documents->current)
     {
      documents->current->update_title();
      cb_play_looped->setChecked (documents->current->wave_edit->waveform->play_looped);
     } 

  //qDebug() << "CEKO::pageChanged end" << index;
}


void CEKO::close_current()
{
  transport_state = STATE_STOP;
  
  if (pa_stream)
     {
      Pa_CloseStream (pa_stream);	
      pa_stream = 0;
     }
     
  documents->close_current();
}


void CEKO::ed_copy()
{
  if (main_tab_widget->currentIndex() == idx_tab_edit)
     {
      CDocument *d = documents->get_current();
      if (d)
        d->wave_edit->waveform->copy_selected();
     }
  else
      if (main_tab_widget->currentIndex() == idx_tab_learn)      
          man->copy();
}


void CEKO::ed_cut()
{
  CDocument *d = documents->get_current();
  if (d)
     d->wave_edit->waveform->cut_selected();
}


void CEKO::ed_paste()
{
  if (! sound_clipboard)
     return;
  
  CDocument *d = documents->get_current();
    
  if (d)
      d->wave_edit->waveform->paste();
}


void CEKO::ed_undo()
{
  CDocument *d = documents->get_current();
  if (d)
     d->wave_edit->waveform->undo_top();
}


void CEKO::ed_redo()
{
  CDocument *d = documents->get_current();
  if (d)
     d->wave_edit->waveform->redo();
}


QAction* CEKO::add_to_menu (QMenu *menu,
                            const QString &caption,
                            const char *method,
                            const QString &shortkt,
                            const QString &iconpath
                           )
{
  QAction *act = new QAction (caption, this);

  if (! shortkt.isEmpty())
     act->setShortcut (shortkt);

  if (! iconpath.isEmpty())
     act->setIcon (QIcon (iconpath));

  connect (act, SIGNAL(triggered()), this, method);
  menu->addAction (act);
  return act;
}


void CEKO::search_find()
{
  if (main_tab_widget->currentIndex() == idx_tab_learn)
      man_find_find();
  else
      if (main_tab_widget->currentIndex() == idx_tab_tune)
         opt_shortcuts_find();
  else
      if (main_tab_widget->currentIndex() == idx_tab_fman)
         fman_find();
}


void CEKO::fman_find()
{
  QString ft = fif_get_text();
  if (ft.isEmpty()) 
      return;  
     
  l_fman_find = fman->mymodel->findItems (ft, Qt::MatchStartsWith);
  
  if (l_fman_find.size() < 1)
     return; 
  
  fman_find_idx = 0;
  
  fman->setCurrentIndex (fman->mymodel->indexFromItem (l_fman_find[fman_find_idx]));
}


void CEKO::fman_find_next()
{
  QString ft = fif_get_text();
  if (ft.isEmpty()) 
      return;  
   
  if (l_fman_find.size() < 1)
     return; 
  
  if (fman_find_idx < (l_fman_find.size() - 1))
     fman_find_idx++;
  
  fman->setCurrentIndex (fman->mymodel->indexFromItem (l_fman_find[fman_find_idx]));
}


void CEKO::fman_find_prev()
{
  QString ft = fif_get_text();
  if (ft.isEmpty()) 
      return;  
   
  if (l_fman_find.size() < 1)
     return; 
  
  if (fman_find_idx != 0)
     fman_find_idx--;
  
  fman->setCurrentIndex (fman->mymodel->indexFromItem (l_fman_find[fman_find_idx]));
}


void CEKO::search_find_next()
{
  if (main_tab_widget->currentIndex() == idx_tab_learn)
      man_find_next();
  else
      if (main_tab_widget->currentIndex() == idx_tab_tune)
          opt_shortcuts_find_next();  
  else
      if (main_tab_widget->currentIndex() == idx_tab_fman)
          fman_find_next();
}


void CEKO::search_find_prev()
{
  if (main_tab_widget->currentIndex() == idx_tab_learn)
          man_find_prev();
  else
      if (main_tab_widget->currentIndex() == idx_tab_tune)
         opt_shortcuts_find_prev();  
  else
      if (main_tab_widget->currentIndex() == idx_tab_fman)
         fman_find_prev();  
}


void CEKO::opt_shortcuts_find()
{
  opt_shortcuts_string_to_find = fif_get_text();
  
  int from = lv_menuitems->currentRow();
  
  if (from == -1)
     from = 0;
  
  int index = shortcuts->captions.indexOf (QRegExp (opt_shortcuts_string_to_find + ".*", Qt::CaseInsensitive), from);
  if (index != -1) 
     lv_menuitems->setCurrentRow (index);
}


void CEKO::opt_shortcuts_find_next()
{
  int from = lv_menuitems->currentRow();
  if (from == -1)
     from = 0;
  
  int index = shortcuts->captions.indexOf (QRegExp (opt_shortcuts_string_to_find + ".*", Qt::CaseInsensitive), from + 1);
  if (index != -1) 
    lv_menuitems->setCurrentRow (index);
}


void CEKO::opt_shortcuts_find_prev()
{
  int from = lv_menuitems->currentRow();
  if (from == -1)
     from = 0;
  
  int index = shortcuts->captions.lastIndexOf (QRegExp (opt_shortcuts_string_to_find + ".*", Qt::CaseInsensitive), from - 1);
  if (index != -1) 
     lv_menuitems->setCurrentRow (index); 
}


void CEKO::slot_lv_menuitems_currentItemChanged (QListWidgetItem *current, QListWidgetItem *previous)
{
  QAction *a = shortcuts->find_by_caption (current->text());
  if (a)
     ent_shtcut->setText (a->shortcut().toString());
}


void CEKO::pb_assign_hotkey_clicked()
{
  if (! lv_menuitems->currentItem())
     return;

  if (ent_shtcut->text().isEmpty())
     return;

  shortcuts->set_new_shortcut (lv_menuitems->currentItem()->text(), ent_shtcut->text());
  shortcuts->save_to_file (shortcuts->fname);
}


void CEKO::pb_remove_hotkey_clicked ()
{
  if (! lv_menuitems->currentItem())
     return;

  shortcuts->set_new_shortcut (lv_menuitems->currentItem()->text(), "");
  ent_shtcut->setText ("");
  shortcuts->save_to_file (shortcuts->fname);
}


void CEKO::slot_sl_icons_size_sliderMoved (int value)
{
  setIconSize (QSize (value, value));
  tb_fman_dir->setIconSize (QSize (value, value));
}


void CEKO::ed_locale_override_editingFinished()
{
  settings->setValue ("override_locale_val", ed_locale_override->text());
}


void CEKO::cmb_sound_dev_out_currentIndexChanged (int index)
{
  settings->setValue ("sound_dev_id_out", index);
  pa_device_id_out = index;
}


void CEKO::cmb_mono_recording_mode_currentIndexChanged (int index)
{
  settings->setValue ("mono_recording_mode", index);
  mono_recording_mode = index;
}


void CEKO::cmb_sound_dev_in_currentIndexChanged (int index)
{
  settings->setValue ("sound_dev_id_in", index);
  pa_device_id_in = index;
}

/*
void CEKO::cmb_panner_currentIndexChanged (int index)
{
  settings->setValue ("panner", index);
  dsp->panner = index;
}
*/

void CEKO::cmb_proxy_video_decoder_currentIndexChanged (int index)
{
  settings->setValue ("proxy_video_decoder", index);
  proxy_video_decoder = index;
}


void CEKO::pb_choose_temp_path_clicked()
{
  QString path = QFileDialog::getExistingDirectory (this, tr ("Open Directory"), "/",
                                                    QFileDialog::ShowDirsOnly |
                                                    QFileDialog::DontResolveSymlinks);
  if (! path.isEmpty())
     {
      settings->setValue ("temp_path", path);
      ed_temp_path->setText (path);
     }
}



void CEKO::createOptions()
{
  tab_options = new QTabWidget;

  idx_tab_tune = main_tab_widget->addTab (tab_options, tr ("tune"));
  
  QWidget *page_interface = new QWidget (tab_options);
  QHBoxLayout *lt_h = new QHBoxLayout;

  QComboBox *cmb_styles = new QComboBox (page_interface);
  cmb_styles->addItems (QStyleFactory::keys());
 
  connect (cmb_styles, SIGNAL(currentIndexChanged (const QString &)),
           this, SLOT(slot_style_currentIndexChanged (const QString &)));

  QLabel *l_app_font = new QLabel (tr ("Interface font"));

  cmb_app_font_name = new QFontComboBox (page_interface);
  cmb_app_font_name->setCurrentFont (QFont (settings->value ("app_font_name", qApp->font().family()).toString()));
  spb_app_font_size = new QSpinBox (page_interface);
  spb_app_font_size->setRange (6, 64);
  QFontInfo fi = QFontInfo (qApp->font());

  spb_app_font_size->setValue (settings->value ("app_font_size", fi.pointSize()).toInt());
  connect (spb_app_font_size, SIGNAL(valueChanged (int)), this, SLOT(slot_app_font_size_changed (int )));

  connect (cmb_app_font_name, SIGNAL(currentIndexChanged ( const QString & )),
           this, SLOT(slot_app_fontname_changed(const QString & )));


  QHBoxLayout *hb_icon_size = new QHBoxLayout;

  QLabel *l_icons_size = new QLabel (tr ("Icons size"));

  QStringList sl_icon_sizes;

  sl_icon_sizes.append ("16");
  sl_icon_sizes.append ("24");
  sl_icon_sizes.append ("32");
  sl_icon_sizes.append ("64");

  QComboBox *cmb_icon_size = new QComboBox;
  cmb_icon_size->addItems (sl_icon_sizes);

  connect (cmb_icon_size, SIGNAL(currentIndexChanged (const QString &)),
           this, SLOT(cmb_icon_sizes_currentIndexChanged (const QString &)));

  cmb_icon_size->setCurrentIndex (sl_icon_sizes.indexOf (settings->value ("icon_size", "32").toString()));

  hb_icon_size->addWidget (l_icons_size);
  hb_icon_size->addWidget (cmb_icon_size);


  QLabel *l_uistyle = new QLabel (tr ("UI style"));
  cmb_styles->setCurrentIndex (cmb_styles->findText (settings->value ("ui_style", "Cleanlooks").toString()));

    
  QCheckBox *cb_show_meterbar_in_db = new QCheckBox (tr ("Amplitude meter bar in dB"), tab_options);
  cb_show_meterbar_in_db->setCheckState (Qt::CheckState (settings->value ("meterbar_show_db", "1").toInt()));
  connect(cb_show_meterbar_in_db, SIGNAL(stateChanged (int )), this, SLOT(cb_show_meterbar_in_db_changed (int )));

  QCheckBox *cb_use_trad_dialogs = new QCheckBox (tr ("Use traditional File Save/Open dialogs"), tab_options);
  cb_use_trad_dialogs->setCheckState (Qt::CheckState (settings->value ("use_trad_dialogs", "0").toInt()));
  connect (cb_use_trad_dialogs, SIGNAL(stateChanged (int )), this, SLOT(cb_use_trad_dialogs_changed (int )));


  QVBoxLayout *page_interface_layout = new QVBoxLayout;
  page_interface_layout->setAlignment (Qt::AlignTop);

  lt_h = new QHBoxLayout;

  lt_h->addWidget (l_app_font);
  lt_h->addWidget (cmb_app_font_name);
  lt_h->addWidget (spb_app_font_size);

  page_interface_layout->addLayout (lt_h);

  page_interface_layout->addLayout (hb_icon_size);

  lt_h = new QHBoxLayout;

  lt_h->addWidget (l_uistyle);
  lt_h->addWidget (cmb_styles);
 
  cb_altmenu = new QCheckBox (tr ("Use Alt key to access main menu"), tab_options);
  if (b_altmenu)
    cb_altmenu->setCheckState (Qt::Checked);
  else
      cb_altmenu->setCheckState (Qt::Unchecked);
  
  connect (cb_altmenu, SIGNAL(stateChanged (int)),
           this, SLOT(cb_altmenu_stateChanged (int)));

   
  page_interface_layout->addLayout (lt_h);
  page_interface_layout->addWidget (cb_show_meterbar_in_db); 
  page_interface_layout->addWidget (cb_use_trad_dialogs);
  page_interface_layout->addWidget (cb_altmenu);
  
 
  page_interface->setLayout (page_interface_layout);
  page_interface->show();

  tab_options->addTab (page_interface, tr ("Interface"));

  //////////
  
  QWidget *page_common = new QWidget (tab_options);
  QVBoxLayout *page_common_layout = new QVBoxLayout;
  page_common_layout->setAlignment (Qt::AlignTop);
  
  
  
  QHBoxLayout *hb_temp_path = new QHBoxLayout;
  QLabel *l_t = new QLabel (tr ("Temp directory"));

  ed_temp_path = new QLineEdit (this);
  ed_temp_path->setText (settings->value ("temp_path", QDir::tempPath()).toString());
  ed_temp_path->setReadOnly (true);

  QPushButton *pb_choose_temp_path = new QPushButton (tr ("Select"), this);

  connect (pb_choose_temp_path, SIGNAL(clicked()), this, SLOT(pb_choose_temp_path_clicked()));

  hb_temp_path->addWidget (l_t);
  hb_temp_path->addWidget (ed_temp_path);
  hb_temp_path->addWidget (pb_choose_temp_path);

  page_common_layout->addLayout (hb_temp_path);
  
  

  QPushButton *bt_set_def_format = new QPushButton (tr ("Set default format for new files"));
  connect (bt_set_def_format, SIGNAL(clicked()), this, SLOT(bt_set_def_format_clicked()));

  QHBoxLayout *hb_mp3_encode = new QHBoxLayout;
  QLabel *l_mp3_encode = new QLabel (tr ("MP3 encode command"));
  ed_mp3_encode = new QLineEdit;
  ed_mp3_encode->setText (settings->value ("mp3_encode", "lame -b 320 -q 0 \"@FILEIN\" \"@FILEOUT\"").toString());
  

  hb_mp3_encode->addWidget (l_mp3_encode);  
  hb_mp3_encode->addWidget (ed_mp3_encode);  


  QHBoxLayout *lt_ogg_q = new QHBoxLayout;

  QLabel *l_ogg_q = new QLabel (tr ("Ogg vorbis quality on saving"));
  spb_ogg_q = new QDoubleSpinBox;
  spb_ogg_q->setRange (0, 1);
  spb_ogg_q->setSingleStep (0.1);
  spb_ogg_q->setValue (settings->value ("ogg_q", 0.5).toDouble());

  lt_ogg_q->addWidget (l_ogg_q);
  lt_ogg_q->addWidget (spb_ogg_q);


/*
  QHBoxLayout *h_panner = new QHBoxLayout;
  QLabel *l_panner = new QLabel (tr ("Panner:"));
    
  cmb_panner = new QComboBox;
  
  QStringList panners;
  panners.append (tr ("linear, law: -6 dB"));
  panners.append (tr ("linear, law: 0 dB"));
  panners.append (tr ("square root, law: -3 dB"));
  panners.append (tr ("sin/cos, law: -3 dB"));
  
  cmb_panner->addItems (panners);
  
  h_panner->addWidget (l_panner);
  h_panner->addWidget (cmb_panner);
  
  cmb_panner->setCurrentIndex (settings->value ("panner", 0).toInt());
  
  
  connect (cmb_panner, SIGNAL(currentIndexChanged (int)),
           this, SLOT(cmb_panner_currentIndexChanged (int)));
*/

 
  QHBoxLayout *h_proxy_video_decoder = new QHBoxLayout;
  QLabel *l_proxy_video_decoder = new QLabel (tr ("MP3 and video decoder (restart EKO to apply):"));
    
  QComboBox *cmb_proxy_video_decoder = new QComboBox;
  
  QStringList sl_video_decoders;
  sl_video_decoders.append ("FFMPEG");
  sl_video_decoders.append ("MPlayer");
  
  cmb_proxy_video_decoder->addItems (sl_video_decoders);
  
  h_proxy_video_decoder->addWidget (l_proxy_video_decoder);
  h_proxy_video_decoder->addWidget (cmb_proxy_video_decoder);
  
  cmb_proxy_video_decoder->setCurrentIndex (settings->value ("proxy_video_decoder", 0).toInt());
  
  
  connect (cmb_proxy_video_decoder, SIGNAL(currentIndexChanged (int)),
           this, SLOT(cmb_proxy_video_decoder_currentIndexChanged (int)));


  QHBoxLayout *lt_maxundos = new QHBoxLayout;
 
  QLabel *l_maxundos = new QLabel (tr ("Max undo items per file"));
  QSpinBox *spb_max_undos = new QSpinBox;
  spb_max_undos->setValue (settings->value ("max_undos", 6).toInt());
  spb_max_undos->setMinimum (1);
  connect (spb_max_undos, SIGNAL(valueChanged (int )), this, SLOT(spb_max_undos_valueChanged (int )));

  lt_maxundos->addWidget (l_maxundos);
  lt_maxundos->addWidget (spb_max_undos);

  QCheckBox *cb_session_restore = new QCheckBox (tr ("Restore the last session on start-up"), tab_options);
  cb_session_restore->setCheckState (Qt::CheckState (settings->value ("session_restore", "0").toInt()));
  connect(cb_session_restore, SIGNAL(stateChanged (int )), this, SLOT(cb_session_restore (int )));
  
  QCheckBox *cb_override_locale = new QCheckBox (tr ("Override locale"), tab_options);
  cb_override_locale->setCheckState (Qt::CheckState (settings->value ("override_locale", 0).toInt()));
  connect(cb_override_locale, SIGNAL(stateChanged (int )), this, SLOT(cb_locale_override (int )));

  ed_locale_override = new QLineEdit (this);
  ed_locale_override->setText (settings->value ("override_locale_val", "en").toString());
  connect(ed_locale_override, SIGNAL(editingFinished()), this, SLOT(ed_locale_override_editingFinished()));

  QHBoxLayout *hb_locovr = new QHBoxLayout;
  
  hb_locovr->addWidget (cb_override_locale);
  hb_locovr->addWidget (ed_locale_override);
  
  page_common_layout->addWidget (bt_set_def_format);

  page_common_layout->addLayout (h_proxy_video_decoder);

  page_common_layout->addLayout (hb_mp3_encode);

  page_common_layout->addLayout (lt_ogg_q);

  page_common_layout->addLayout (lt_maxundos);
  
  //page_common_layout->addLayout (h_panner);
      
  page_common_layout->addWidget (cb_session_restore);
    
  page_common_layout->addLayout (hb_locovr);
      
  
  page_common->setLayout (page_common_layout);
  page_common->show();

  tab_options->addTab (page_common, tr ("Common"));

  
/////////////

  QWidget *page_soundev = new QWidget (tab_options);
  QVBoxLayout *page_soundev_layout = new QVBoxLayout;
  page_soundev_layout->setAlignment (Qt::AlignTop);

  page_soundev->setLayout (page_soundev_layout);
  page_soundev->show();

  tab_options->addTab (page_soundev, tr ("Sound devices"));


  QHBoxLayout *hb_soundev = new QHBoxLayout;

  pa_device_id_out = settings->value ("sound_dev_id_out", Pa_GetDefaultOutputDevice()).toInt();
  pa_device_id_in = settings->value ("sound_dev_id_in", Pa_GetDefaultInputDevice()).toInt();
  
  QLabel *l_soundev = new QLabel (tr ("Output"));
  
  cmb_sound_dev_out = new QComboBox;
  cmb_sound_dev_out->addItems (get_sound_devices());
  
  hb_soundev->addWidget (l_soundev);
  hb_soundev->addWidget (cmb_sound_dev_out);



  QHBoxLayout *hb_soundev_in = new QHBoxLayout;

  QLabel *l_soundev_in = new QLabel (tr ("Input"));

  cmb_sound_dev_in = new QComboBox;
  cmb_sound_dev_in->addItems (get_sound_devices());
  
  hb_soundev_in->addWidget (l_soundev_in);
  hb_soundev_in->addWidget (cmb_sound_dev_in);

  
  cmb_sound_dev_in->setCurrentIndex (pa_device_id_in);
  cmb_sound_dev_out->setCurrentIndex (pa_device_id_out);
  
  
  connect (cmb_sound_dev_in, SIGNAL(currentIndexChanged (int)),
           this, SLOT(cmb_sound_dev_in_currentIndexChanged (int)));

  connect (cmb_sound_dev_out, SIGNAL(currentIndexChanged (int)),
           this, SLOT(cmb_sound_dev_out_currentIndexChanged (int)));



  QHBoxLayout *hb_mono_recording_mode = new QHBoxLayout;

  QLabel *l_mono_recording_mode = new QLabel (tr ("Mono recording mode"));

  QComboBox *cmb_mono_recording_mode = new QComboBox;
  
  QStringList lmrm;
  
 // lmrm.append (tr ("Mix both channels"));
  lmrm.append (tr ("Use left channel"));
  lmrm.append (tr ("Use right channel"));
  
  cmb_mono_recording_mode->addItems (lmrm);
  
  hb_mono_recording_mode->addWidget (l_mono_recording_mode);
  hb_mono_recording_mode->addWidget (cmb_mono_recording_mode);

  connect (cmb_mono_recording_mode, SIGNAL(currentIndexChanged (int)),
           this, SLOT(cmb_mono_recording_mode_currentIndexChanged (int)));
  
  cmb_mono_recording_mode->setCurrentIndex (settings->value ("mono_recording_mode", 0).toInt());


  QHBoxLayout *hb_buffer_size_frames = new QHBoxLayout;

  QLabel *l_buffer_size_frames = new QLabel (tr ("Buffer size (in frames)"));
  
  cmb_buffer_size_frames = new QComboBox;
  
  QStringList lsizes;
  
  lsizes.append ("128");
  lsizes.append ("256");
  lsizes.append ("512");
  lsizes.append ("1024");
  lsizes.append ("2048");
  lsizes.append ("4096");
  
  cmb_buffer_size_frames->addItems (lsizes);
  
  
  hb_buffer_size_frames->addWidget (l_buffer_size_frames);
  hb_buffer_size_frames->addWidget (cmb_buffer_size_frames);

  int idx = lsizes.indexOf (settings->value ("buffer_size_frames", "2048").toString());

  cmb_buffer_size_frames->setCurrentIndex (idx);
 
  
  connect (cmb_buffer_size_frames, SIGNAL(currentIndexChanged (const QString &)),
           this, SLOT(cmb_buffer_size_frames_currentIndexChanged (const QString &)));
  
  
  
  QCheckBox *cb_monitor_input = new QCheckBox (tr ("Monitor input"), tab_options);
  cb_monitor_input->setCheckState (Qt::CheckState (settings->value ("b_monitor_input", 0).toInt()));
  connect(cb_monitor_input, SIGNAL(stateChanged (int )), this, SLOT(cb_monitor_input_changed (int )));


  
  page_soundev_layout->addLayout (hb_soundev_in);
  page_soundev_layout->addLayout (hb_soundev);
  page_soundev_layout->addLayout (hb_mono_recording_mode);

  page_soundev_layout->addLayout (hb_buffer_size_frames);
  page_soundev_layout->addWidget (cb_monitor_input);
  

//////////

  QWidget *page_keyboard = new QWidget (tab_options);

  lt_h = new QHBoxLayout;

  QVBoxLayout *lt_vkeys = new QVBoxLayout;
  QVBoxLayout *lt_vbuttons = new QVBoxLayout;

  lv_menuitems = new QListWidget;
  opt_update_keyb();
  
  lt_vkeys->addWidget (lv_menuitems);

  connect (lv_menuitems, SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)),
           this, SLOT(slot_lv_menuitems_currentItemChanged (QListWidgetItem *, QListWidgetItem *)));

  ent_shtcut = new CShortcutEntry;
  lt_vbuttons->addWidget (ent_shtcut);

  QPushButton *pb_assign_hotkey = new QPushButton (tr ("Assign"), this);
  QPushButton *pb_remove_hotkey = new QPushButton (tr ("Remove"), this);

  connect (pb_assign_hotkey, SIGNAL(clicked()), this, SLOT(pb_assign_hotkey_clicked()));
  connect (pb_remove_hotkey, SIGNAL(clicked()), this, SLOT(pb_remove_hotkey_clicked()));

  lt_vbuttons->addWidget (pb_assign_hotkey);
  lt_vbuttons->addWidget (pb_remove_hotkey, 0, Qt::AlignTop);

  lt_h->addLayout (lt_vkeys);
  lt_h->addLayout (lt_vbuttons);

  page_keyboard->setLayout (lt_h);
  page_keyboard->show();

  tab_options->addTab (page_keyboard, tr ("Keyboard"));
}


void CEKO::opt_update_keyb()
{
   if (! lv_menuitems)
      return;
   
   shortcuts->captions_iterate();
   lv_menuitems->clear();
   lv_menuitems->addItems (shortcuts->captions);
}


void CEKO::slot_style_currentIndexChanged (const QString &text)
{
   if (text == "GTK+") //because it is buggy with some Qt versions. sorry!
     return;
  
  MyProxyStyle *ps = new MyProxyStyle (QStyleFactory::create (text));
  
  QApplication::setStyle (ps);

  settings->setValue ("ui_style", text);
}


void CEKO::cb_session_restore (int state)
{
  settings->setValue ("session_restore", state);
}


void CEKO::cb_use_trad_dialogs_changed (int state)
{
  settings->setValue ("use_trad_dialogs", state);
}


void CEKO::cb_locale_override (int state)
{
  settings->setValue ("override_locale", state);
}


void CEKO::cb_monitor_input_changed (int state)
{
  settings->setValue ("b_monitor_input", state);
  b_monitor_input = bool (state);
}


void CEKO::slot_app_fontname_changed (const QString &text)
{
  settings->setValue ("app_font_name", text);
  update_stylesheet (fname_stylesheet);
}


void CEKO::slot_app_font_size_changed (int i)
{
  settings->setValue("app_font_size", i);
  update_stylesheet (fname_stylesheet);
}


void CEKO::updateFonts()
{
  documents->apply_settings();

  QFont fapp;
  QFontInfo fi = QFontInfo (qApp->font());

  fapp.setPointSize (settings->value ("app_font_size", fi.pointSize()).toInt());
  fapp.fromString (settings->value ("app_font_name", qApp->font().family()).toString());
  qApp->setFont (fapp);
}


void CEKO::man_find_find()
{
  man_search_value = fif_get_text();
  man->find (man_search_value);
}


void CEKO::man_find_next()
{
  man->find (man_search_value);
}


void CEKO::man_find_prev()
{
  man->find (man_search_value, QTextDocument::FindBackward);
}


void CEKO::createManual()
{
  QWidget *wd_man = new QWidget (this);

  QVBoxLayout *lv_t = new QVBoxLayout;

  QString loc = QLocale::system().name().left (2);

  if (settings->value ("override_locale", 0).toBool())
     {
      QString ts = settings->value ("override_locale_val", "en").toString();
      if (ts.length() != 2)
          ts = "en";
      loc = ts;
     }

  QString filename (":/manuals/");
  filename.append (loc).append (".html");
  
  if (! file_exists (filename))
      filename = ":/manuals/en.html";

  man_search_value = "";

  QHBoxLayout *lh_controls = new QHBoxLayout();

  QPushButton *bt_back = new QPushButton ("<");
  QPushButton *bt_forw = new QPushButton (">");

  lh_controls->addWidget (bt_back);
  lh_controls->addWidget (bt_forw);

  man = new QTextBrowser;
  man->setOpenExternalLinks (true);
  man->setSource (QUrl ("qrc" + filename));

  connect (bt_back, SIGNAL(clicked()), man, SLOT(backward()));
  connect (bt_forw, SIGNAL(clicked()), man, SLOT(forward()));

  lv_t->addLayout (lh_controls);
  lv_t->addWidget (man);

  wd_man->setLayout (lv_t);

  idx_tab_learn = main_tab_widget->addTab (wd_man, tr ("learn"));
}


void CEKO::file_last_opened()
{
  if (documents->recent_files.size() > 0)
     {
      documents->open_file (documents->recent_files[0]);
      documents->recent_files.removeAt (0);
      documents->update_recent_menu();
      main_tab_widget->setCurrentIndex (idx_tab_edit); 
     }
}


void CEKO::file_save_version()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  if (! file_exists (d->file_name))
     return;

  QDate date = QDate::currentDate();
  QFileInfo fi;
  fi.setFile (d->file_name);

  QString version_timestamp_fmt = settings->value ("version_timestamp_fmt", "yyyy-MM-dd").toString();
  QTime t = QTime::currentTime();

  QString fname = QString ("%1/%2-%3-%4.%5").arg (
                           fi.absoluteDir().absolutePath()).arg (
                           date.toString (version_timestamp_fmt)).arg (
                           t.toString("hh-mm-ss")).arg (
                           fi.baseName()).arg (
                           fi.suffix());

  if (d->save_with_name_plain (fname))
     log->log (tr ("%1 - saved").arg (fname));
  else
     log->log (tr ("Cannot save %1").arg (fname));
}


void CEKO::dragEnterEvent (QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat ("text/uri-list"))
     event->acceptProposedAction();
}


void CEKO::dropEvent (QDropEvent *event)
{
  QString fName;
  QFileInfo info;
  
  if (! event->mimeData()->hasUrls())
     return;
     
  foreach (QUrl u, event->mimeData()->urls())     
          {
           fName = u.toLocalFile();
           info.setFile (fName);
           if (info.isFile())
               documents->open_file (fName);
           }
              
  event->acceptProposedAction();
}


void CApplication::saveState (QSessionManager &manager)
{
  manager.setRestartHint (QSessionManager::RestartIfRunning);
}


void CEKO::nav_goto_right_tab()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  if (tab_widget->currentIndex() > (tab_widget->count() - 1))
    return;

  tab_widget->setCurrentIndex (tab_widget->currentIndex() + 1);
}


void CEKO::nav_goto_left_tab()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  if (tab_widget->currentIndex() == 0)
     return;

  tab_widget->setCurrentIndex (tab_widget->currentIndex() - 1);
}


void CEKO::nav_focus_to_fif()
{
  fif->setFocus (Qt::OtherFocusReason);
}


void CEKO::nav_focus_to_editor()
{
  CDocument *d = documents->get_current();
  if (d)
     d->wave_edit->setFocus (Qt::OtherFocusReason);
}


void CEKO::edit_copy_current_fname()
{
  CDocument *d = documents->get_current();
  if (d)
     QApplication::clipboard()->setText (d->file_name);
}


void CEKO::help_show_gpl()
{
  show_text_file (":/COPYING");
}


void CEKO::update_dyn_menus()
{
  update_palettes();
  opt_update_keyb();
  update_profiles();
  update_themes();
}


void CEKO::view_toggle_fs()
{
  setWindowState (windowState() ^ Qt::WindowFullScreen);
}


void CEKO::help_show_news()
{
  QString fname = ":/NEWS";
  if (QLocale::system().name().left(2) == "ru")
     fname = ":/NEWS-RU";

  show_text_file (fname);
}


void CEKO::help_show_changelog()
{
  show_text_file (":/ChangeLog");
}


void CEKO::help_show_todo()
{
  show_text_file (":/TODO");
}


void CAboutWindow::closeEvent (QCloseEvent *event)
{
  event->accept();
}


void CAboutWindow::update_image()
{
  QImage img (400, 128, QImage::Format_RGB32);

  QPainter painter (&img);

  for (int y = 1; y < 128 ; y += 16)
  for (int x = 1; x < 400; x += 25)
      {
       QColor color;
       
       int i = qrand() % 5;
       
       switch (i)
              {
               case 0: color = 0xfff3f9ff;
                       break;

               case 1: color = 0xffbfffb0;
                       break;

               case 2: color = 0xffa5a5a6;
                       break;
                       
               case 3: color = 0xffebffe9;
                       break;
               
               case 4: color = 0xffbbf6ff;
                       break;
              }
                  
       painter.fillRect (x, y, 25, 16, QBrush (color));
       
       if (i == 0) 
          {
           QFont f;
           f.setPixelSize (16);
           painter.setPen (Qt::gray);
           painter.setFont (f);
           painter.drawText (x, y + 16, "0");
          }

        if (i == 1) 
           {
            QFont f;
            f.setPixelSize (16);
            painter.setFont (f);
            painter.setPen (Qt::gray);
            painter.drawText (x, y + 16, "1");
           }
     } 

  QString txt = "EKO";
  
  QFont f ("Monospace");
  f.setPixelSize (85);
  painter.setFont (f);
  
  painter.setPen (Qt::black);
  painter.drawText (4, 90, txt);
  
  painter.setPen (Qt::red);
  painter.drawText (2, 86, txt);

  if (mascot_x > width() - mascot.width())
     mascot_right = false;
  if (mascot_x < 100)
     mascot_right = true;
     
  if (mascot_right)   
      mascot_x += 10;  
  else
      mascot_x -= 10;  
      
  painter.drawImage (mascot_x, 1, mascot);

  logo->setPixmap (QPixmap::fromImage (img));
}
  

CAboutWindow::CAboutWindow()
{
  setAttribute (Qt::WA_DeleteOnClose);

  mascot.load (":/icons/eko_icon.png");
  mascot_x = 100;
  mascot_right = true;

  QStringList sl_t = qstring_load (":/AUTHORS").split ("##");

  logo = new QLabel;
  update_image();
    
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update_image()));
  timer->start (500);
  
  QTabWidget *tw = new QTabWidget (this);

  QPlainTextEdit *page_code = new QPlainTextEdit();
  QPlainTextEdit *page_thanks = new QPlainTextEdit();
  QPlainTextEdit *page_translators = new QPlainTextEdit();
  QPlainTextEdit *page_maintainers = new QPlainTextEdit();

  if (sl_t.size() == 4)
     {
      page_code->setPlainText (sl_t[0].trimmed());
      page_thanks->setPlainText (sl_t[3].trimmed());
      page_translators->setPlainText (sl_t[1].trimmed());
      page_maintainers->setPlainText (sl_t[2].trimmed());
     }

  tw->addTab (page_code, tr ("Code"));
  tw->addTab (page_thanks, tr ("Acknowledgements"));
  tw->addTab (page_translators, tr ("Translations"));
  tw->addTab (page_maintainers, tr ("Packages"));

  QVBoxLayout *layout = new QVBoxLayout();

  layout->addWidget(logo);
  layout->addWidget(tw);

  setLayout (layout);
  setWindowTitle (tr ("About"));
}


void CEKO::cb_button_saves_as()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  if (ed_fman_fname->text().isEmpty())
     return;

  QString filename (fman->dir.path());
  
  filename.append ("/").append (ed_fman_fname->text());

  if (file_exists (filename))
     if (QMessageBox::warning (this, "EKO",
                               tr ("%1 already exists\n"
                               "Do you want to overwrite?")
                               .arg (filename),
                               QMessageBox::Yes | QMessageBox::Default,
                               QMessageBox::Cancel | QMessageBox::Escape) == QMessageBox::Cancel)
         return;


   d->save_with_name (filename);
   
   QFileInfo f (d->file_name);
   dir_last = f.path();
   update_dyn_menus();
   fman->refresh();
   main_tab_widget->setCurrentIndex (idx_tab_edit);
}


void CEKO::fman_home()
{
#if defined(Q_OS_WIN) || defined(Q_OS_OS2)

  fman->nav ("c:/");

#else

  fman->nav (QDir::homePath());

#endif
}


void CEKO::fman_add_bmk()
{
  sl_places_bmx.prepend (ed_fman_path->text());
  qstring_save (fname_places_bookmarks, sl_places_bmx.join ("\n"));
  update_places_bookmarks();
}


void CEKO::fman_del_bmk()
{
  int i = lv_places->currentRow();

  if (i == -1)
     return; 

  QString s = lv_places->item(i)->text();
  if (s.isNull() || s.isEmpty())
     return;

  i = sl_places_bmx.indexOf (s);
  sl_places_bmx.removeAt (i);
  qstring_save (fname_places_bookmarks, sl_places_bmx.join ("\n"));
  update_places_bookmarks();
}


void CEKO::fman_naventry_confirm()
{
  fman->nav (ed_fman_path->text());
}


void CEKO::fman_places_itemActivated (QListWidgetItem *item)
{
  fman->nav (item->text());
}


void CEKO::update_places_bookmarks()
{
  lv_places->clear();
  QStringList sl_items;
  
  if (! file_exists (fname_places_bookmarks))
     return;

  sl_places_bmx = qstring_load (fname_places_bookmarks).split ("\n");
  if (sl_places_bmx.size() != 0)
     lv_places->addItems (sl_places_bmx);
}


void CEKO::fman_open()
{
  main_tab_widget->setCurrentIndex (idx_tab_edit);
 
  QString f = ed_fman_fname->text().trimmed();
  QStringList li = fman->get_sel_fnames();

  if (! f.isNull() || ! f.isEmpty())
  if (f[0] == '/')
     {
      CDocument *d = documents->open_file (f);

      main_tab_widget->setCurrentIndex (idx_tab_edit);
      return;
     }

  if (li.size() == 0)
     {
      QString fname (fman->dir.path());
      fname.append ("/").append (f);
      CDocument *d = documents->open_file (fname);
      main_tab_widget->setCurrentIndex (idx_tab_edit);
      return;
     }

  foreach (QString fname, li)
          {
           CDocument *d = 0;
           d = documents->open_file (fname);
           if (d)
               dir_last = get_file_path (d->file_name);
          }
}


void CEKO::fman_create_dir()
{
  bool ok;
  QString newdir = QInputDialog::getText (this, tr ("Enter the name"),
                                                tr ("Name:"), QLineEdit::Normal,
                                                tr ("new_directory"), &ok);
  if (! ok || newdir.isEmpty())
     return;

  QString dname = fman->dir.path() + "/" + newdir;

  QDir d;
  if (! d.mkpath (dname))
     return;

  fman->nav (dname);
}


void CEKO::view_show_mixer()
{
  bool visibility = ! wnd_fxrack->isVisible();
  wnd_fxrack->setVisible (visibility);
  settings->setValue ("wnd_fxrack_visible", visibility);
}


void CEKO::view_stay_on_top()
{
  Qt::WindowFlags flags = windowFlags();
  flags ^= Qt::WindowStaysOnTopHint;
  setWindowFlags (flags );
  show();
  activateWindow();
}


void CEKO::update_sessions()
{
  menu_file_sessions->clear();
  create_menu_from_dir (this,
                        menu_file_sessions,
                        dir_sessions,
                        SLOT (file_open_session())
                       );
}


void CEKO::file_open_session()
{
  QAction *a = qobject_cast<QAction *>(sender());
  documents->load_from_session (a->data().toString());
}


void CEKO::session_save_as()
{
  if (documents->list.size() < 0)
     return;

  bool ok;
  QString name = QInputDialog::getText (this, tr ("Enter the name"),
                                              tr ("Name:"), QLineEdit::Normal,
                                              tr ("new_session"), &ok);
  if (! ok || name.isEmpty())
     return;

  QString fname (dir_sessions);
  fname.append ("/").append (name);
  documents->save_to_session (fname);
  update_sessions();
}


void CEKO::load_palette (const QString &fileName)
{
  if (! file_exists (fileName))
      return;

  global_palette.clear();
  global_palette = hash_load_keyval (fileName);
}



void CEKO::file_use_palette()
{
  QAction *a = qobject_cast<QAction *>(sender());
  QString fname (dir_palettes);
  fname.append ("/").append (a->text());

  if (! file_exists (fname))
     fname = ":/palettes/" + a->text();

  fname_def_palette = fname;
  load_palette (fname);
  
  documents->def_palette = fname;
  documents->load_palette (fname);
  update_stylesheet (fname_stylesheet);
  
  documents->apply_settings();
}


void CEKO::update_palettes()
{
  menu_view_palettes->clear();

  QStringList l1 = read_dir_entries (dir_palettes);
  QStringList l2 = read_dir_entries (":/palettes");
  l1 += l2;

  create_menu_from_list (this, menu_view_palettes,
                         l1,
                         SLOT (file_use_palette()));
}


void CEKO::fman_drives_changed (const QString & path)
{
  if (! ui_update)
     fman->nav (path);
}


void CEKO::createFman()
{
  QWidget *wd_fman = new QWidget (this);

  QVBoxLayout *lav_main = new QVBoxLayout;
  QVBoxLayout *lah_controls = new QVBoxLayout;
  QHBoxLayout *lah_topbar = new QHBoxLayout;

  QLabel *l_t = new QLabel (tr ("Name"));
  ed_fman_fname = new QLineEdit;
  connect (ed_fman_fname, SIGNAL(returnPressed()), this, SLOT(fman_fname_entry_confirm()));

  
  ed_fman_path = new QLineEdit;
  connect (ed_fman_path, SIGNAL(returnPressed()), this, SLOT(fman_naventry_confirm()));

  tb_fman_dir = new QToolBar;
  tb_fman_dir->setObjectName ("tb_fman_dir");

  QAction *act_fman_go = new QAction (QIcon( ":/icons/go.png"), tr ("Go"), this);
  connect (act_fman_go, SIGNAL(triggered()), this, SLOT(fman_naventry_confirm()));

  QAction *act_fman_home = new QAction (QIcon (":/icons/home.png"), tr ("Home"), this);
  connect (act_fman_home, SIGNAL(triggered()), this, SLOT(fman_home()));

  QAction *act_fman_refresh = new QAction (QIcon (":/icons/refresh.png"), tr ("Refresh"), this);
  QAction *act_fman_ops = new QAction (QIcon (":/icons/create-dir.png"), tr ("Operations"), this);
  act_fman_ops->setMenu (menu_fm_file_ops);

  tb_fman_dir->addAction (act_fman_go);
  tb_fman_dir->addAction (act_fman_home);
  tb_fman_dir->addAction (act_fman_refresh);
  tb_fman_dir->addAction (act_fman_ops);

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)

  cb_fman_drives = new QComboBox;
  lah_topbar->addWidget (cb_fman_drives);

  QFileInfoList l_drives = QDir::drives();
  foreach (QFileInfo fi, l_drives)
           cb_fman_drives->addItem (fi.path());
  
#endif

  lah_topbar->addWidget (ed_fman_path);
  lah_topbar->addWidget (tb_fman_dir);

  lah_controls->addWidget (l_t);
  lah_controls->addWidget (ed_fman_fname);

  
  QPushButton *bt_fman_open = new QPushButton (tr ("Open"), this);
  connect (bt_fman_open, SIGNAL(clicked()), this, SLOT(fman_open()));

  QPushButton *bt_fman_save_as = new QPushButton (tr ("Save as"), this);
  connect (bt_fman_save_as, SIGNAL(clicked()), this, SLOT(cb_button_saves_as()));

  lah_controls->addWidget (l_t);
  lah_controls->addWidget (bt_fman_open);
  lah_controls->addWidget (bt_fman_save_as);

  fman = new CFMan;

  connect (fman, SIGNAL(file_activated (const QString &)), this, SLOT(fman_file_activated (const QString &)));
  connect (fman, SIGNAL(dir_changed  (const QString &)), this, SLOT(fman_dir_changed  (const QString &)));
  connect (fman, SIGNAL(current_file_changed  (const QString &, const QString &)), this, SLOT(fman_current_file_changed  (const QString &, const QString &)));

  connect (act_fman_refresh, SIGNAL(triggered()), fman, SLOT(refresh()));

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)

  connect (cb_fman_drives, SIGNAL(currentIndexChanged ( const QString & )),
          this, SLOT(fman_drives_changed(const QString & )));

#endif

  w_right = new QWidget (this);
  
  w_right->setMinimumWidth (10);

  
  QVBoxLayout *lw_right = new QVBoxLayout;
  w_right->setLayout (lw_right);  
  
  lw_right->addLayout (lah_controls);

  QFrame *vline = new QFrame;
  vline->setFrameStyle (QFrame::HLine);
  lw_right->addWidget (vline);

  QLabel *l_bookmarks = new QLabel (tr ("<b>Bookmarks</b>"));
  lw_right->addWidget (l_bookmarks);


  QHBoxLayout *lah_places_bar = new QHBoxLayout;
  QPushButton *bt_add_bmk = new QPushButton ("+");
  QPushButton *bt_del_bmk = new QPushButton ("-");
  lah_places_bar->addWidget (bt_add_bmk);
  lah_places_bar->addWidget (bt_del_bmk);

  connect (bt_add_bmk, SIGNAL(clicked()), this, SLOT(fman_add_bmk()));
  connect (bt_del_bmk, SIGNAL(clicked()), this, SLOT(fman_del_bmk()));

  lv_places = new QListWidget;
  update_places_bookmarks();
  connect (lv_places, SIGNAL(itemActivated (QListWidgetItem *)), this, SLOT(fman_places_itemActivated (QListWidgetItem *)));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addLayout (lah_places_bar);
  vbox->addWidget (lv_places);

  lw_right->addLayout (vbox);
  
  fman->setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

  spl_fman = new QSplitter (this);
  spl_fman->setChildrenCollapsible (true);

  spl_fman->addWidget (fman);
  spl_fman->addWidget (w_right);
  
  spl_fman->restoreState (settings->value ("spl_fman").toByteArray());

  lav_main->addLayout (lah_topbar);
  lav_main->addWidget (spl_fman);

  wd_fman->setLayout (lav_main);

  fman_home();
  
  idx_tab_fman = main_tab_widget->addTab (wd_fman, tr ("manage"));
}


void CEKO::fman_file_activated (const QString &full_path)
{
  main_tab_widget->setCurrentIndex (idx_tab_edit);

  transport_state = STATE_STOP;
 
  CDocument *d = documents->open_file (full_path);
  if (d)
      dir_last = get_file_path (d->file_name);
}


void CEKO::fman_dir_changed (const QString &full_path)
{
  ui_update = true;
  ed_fman_path->setText (full_path);

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)

  cb_fman_drives->setCurrentIndex (cb_fman_drives->findText (full_path.left(3).toUpper()));

#endif

  ui_update = false;
}


void CEKO::fman_current_file_changed (const QString &full_path, const QString &just_name)
{
  ed_fman_fname->setText (just_name);
}


void CEKO::fman_rename()
{
 QString fname = fman->get_sel_fname();
  if (fname.isNull() || fname.isEmpty())
     return;
  
  QFileInfo fi (fname);
  if (! fi.exists() && ! fi.isWritable())
     return;
  
  bool ok;
  QString newname = QInputDialog::getText (this, tr ("Enter the name"),
                                                 tr ("Name:"), QLineEdit::Normal,
                                                 tr ("new"), &ok);
  if (! ok || newname.isEmpty())
     return;
  
  QString newfpath (fi.path());
  newfpath.append ("/").append (newname);
  QFile::rename (fname, newfpath);
  update_dyn_menus();
  fman->refresh();

  QModelIndex index = fman->index_from_name (newname);
  fman->selectionModel()->setCurrentIndex (index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  fman->scrollTo (index, QAbstractItemView::PositionAtCenter);
}


void CEKO::fman_delete()
{
  QString fname = fman->get_sel_fname();
  if (fname.isNull() || fname.isEmpty())
     return;

  int i = fman->get_sel_index();

  QFileInfo fi (fname);
  if (! fi.exists() && ! fi.isWritable())
     return;
 
  if (QMessageBox::warning (this, "EKO",
                            tr ("Are you sure to delete\n"
                            "%1?").arg (fname),
                            QMessageBox::Yes | QMessageBox::Default,
                            QMessageBox::No | QMessageBox::Escape) == QMessageBox::No)
      return;
 
  QFile::remove (fname);
  update_dyn_menus();
  fman->refresh();

  if (i < fman->list.count())
     {
      QModelIndex index = fman->index_from_idx (i);
      fman->selectionModel()->setCurrentIndex (index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      fman->scrollTo (index, QAbstractItemView::PositionAtCenter);
     }
}


void CEKO::fman_refresh()
{
  fman->refresh();
}


void CEKO::fm_full_info()
{
  QString fname;

  if (main_tab_widget->currentIndex() == idx_tab_fman)
     fname = fman->get_sel_fname();
  else
      {
       CDocument *d = documents->get_current();
       if (d)
          fname = d->file_name;
      }

  QFileInfo fi (fname);
  if (! fi.exists())
     return;

  QStringList l;

  l.append (tr ("file name: %1").arg (fi.absoluteFilePath()));
  l.append (tr ("size: %1 kbytes").arg (QString::number (fi.size() / 1024)));
  l.append (tr ("created: %1").arg (fi.created().toString ("yyyy-MM-dd@hh:mm:ss")));
  l.append (tr ("modified: %1").arg (fi.lastModified().toString ("yyyy-MM-dd@hh:mm:ss")));
  
  log->log (l.join ("<br>"));
}


void CEKO::file_reload()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  d->reload();
}


void CEKO::handle_args()
{
  QStringList l = qApp->arguments();
  int size = l.size();
  if (size < 2)
     return;

  for (int i = 1; i < size; i++)
      {
       QFileInfo f (l.at(i));

       if (! f.isAbsolute())
          {
           QString fullname (QDir::currentPath());
           fullname.append ("/").append (l.at(i));
           documents->open_file (fullname);
          }
       else
            documents->open_file (l.at(i));
      }
      
   transport_state = STATE_STOP;
}


void CEKO::main_tab_page_changed (int index)
{
  if (idx_prev == idx_tab_tune)
     leaving_tune();

  idx_prev = index;

  if (index == idx_tab_fman)
     {
      fman->setFocus();
      fm_entry_mode = FM_ENTRY_MODE_NONE;
      idx_tab_fman_activate();
     }
  else       
  if (index == idx_tab_edit)
     idx_tab_edit_activate();
  else   
  if (index == idx_tab_tune)
     idx_tab_tune_activate();
  else   
  if (index == idx_tab_learn)
     idx_tab_learn_activate();
}


QString CEKO::fif_get_text()
{
  QString t = fif->text();

  int i = sl_fif_history.indexOf (t);

  if (i != -1)
     {
      sl_fif_history.removeAt (i);
      sl_fif_history.prepend (t);
     }
  else
      sl_fif_history.prepend (t);

  if (sl_fif_history.count() > 77)
     sl_fif_history.removeLast();

  return t;
}


void CEKO::fman_fname_entry_confirm()
{
  if (fm_entry_mode == FM_ENTRY_MODE_OPEN)
     fman_open();     
   
  if (fm_entry_mode == FM_ENTRY_MODE_SAVE)
     cb_button_saves_as();
}


void CEKO::view_use_profile()
{
  QAction *a = qobject_cast<QAction *>(sender());
  QSettings s (a->data().toString(), QSettings::IniFormat);

  QPoint pos = s.value ("pos", QPoint (1, 200)).toPoint();
  QSize size = s.value ("size", QSize (600, 420)).toSize();
  mainSplitter->restoreState (s.value ("splitterSizes").toByteArray());
  resize (size);
  move (pos);
  
  documents->apply_settings();  
}


void CEKO::profile_save_as()
{
  bool ok;
  QString name = QInputDialog::getText (this, tr ("Enter the name"),
                                              tr ("Name:"), QLineEdit::Normal,
                                              tr ("new_profile"), &ok);
  if (! ok || name.isEmpty())
     return;

  QString fname (dir_profiles);
  fname.append ("/").append (name);

  QSettings s (fname, QSettings::IniFormat);

  s.setValue ("pos", pos());
  s.setValue ("size", size());
  s.setValue ("splitterSizes", mainSplitter->saveState());

  s.sync();
  
  update_profiles();
}


void CEKO::update_profiles()
{
  menu_view_profiles->clear();
  create_menu_from_dir (this,
                        menu_view_profiles,
                        dir_profiles,
                        SLOT (view_use_profile())
                       );
}


void CEKO::fman_items_select_by_regexp (bool mode)
{
  QString ft = fif_get_text();
  if (ft.isEmpty()) 
      return;  
     
  l_fman_find = fman->mymodel->findItems (ft, Qt::MatchRegExp);
  
  if (l_fman_find.size() < 1)
     return; 
   
  QItemSelectionModel *m = fman->selectionModel();
  for (int i = 0; i < l_fman_find.size(); i++)
      if (mode) 
         m->select (fman->mymodel->indexFromItem (l_fman_find[i]), QItemSelectionModel::Select | QItemSelectionModel::Rows);
      else   
          m->select (fman->mymodel->indexFromItem (l_fman_find[i]), QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}


void CEKO::fman_select_by_regexp()
{
  fman_items_select_by_regexp (true);
}


void CEKO::fman_deselect_by_regexp()
{
  fman_items_select_by_regexp (false);
}

  
void CEKO::idx_tab_edit_activate()
{
  fileMenu->menuAction()->setVisible (true);
  editMenu->menuAction()->setVisible (true);
  menu_functions->menuAction()->setVisible (true);
  menu_search->menuAction()->setVisible (false);
  menu_nav->menuAction()->setVisible (true);
  menu_fm->menuAction()->setVisible (false);
  menu_view->menuAction()->setVisible (true);
  helpMenu->menuAction()->setVisible (true);
}


void CEKO::idx_tab_tune_activate()
{
  fileMenu->menuAction()->setVisible (true);
  editMenu->menuAction()->setVisible (false);
  menu_functions->menuAction()->setVisible (true);
  menu_search->menuAction()->setVisible (false);
  menu_nav->menuAction()->setVisible (false);
  menu_fm->menuAction()->setVisible (false);
  menu_view->menuAction()->setVisible (true);
  helpMenu->menuAction()->setVisible (true);
}


void CEKO::idx_tab_fman_activate()
{
  fileMenu->menuAction()->setVisible (true);
  editMenu->menuAction()->setVisible (false);
  menu_functions->menuAction()->setVisible (false);
  menu_search->menuAction()->setVisible (true);
  menu_nav->menuAction()->setVisible (false);
  menu_fm->menuAction()->setVisible (true);
  menu_view->menuAction()->setVisible (true);
  helpMenu->menuAction()->setVisible (true);
}


void CEKO::idx_tab_learn_activate()
{
  fileMenu->menuAction()->setVisible (true);
  editMenu->menuAction()->setVisible (false);
  menu_functions->menuAction()->setVisible (false);
  menu_search->menuAction()->setVisible (true);
  menu_nav->menuAction()->setVisible (false);
  menu_fm->menuAction()->setVisible (false);
  menu_view->menuAction()->setVisible (true);
  helpMenu->menuAction()->setVisible (true);
}


void CEKO::show_text_file (const QString &fname)
{
  text_file_browser->setPlainText (qstring_load (fname));
  text_file_browser->resize (width(), height());
  text_file_browser->show();
}


void CEKO::show_html_data (const QString &data)
{
  text_file_browser->setHtml (data);
  text_file_browser->resize (width(), height());
  text_file_browser->show();
}


void CEKO::slot_transport_play()
{
  if (transport_state == STATE_RECORD)
     return;

  if (pa_stream)
     {
      Pa_CloseStream (pa_stream);	
      pa_stream = 0;
     }
  
  CDocument *d = documents->get_current(); 
  
  if (! d)
     return;
    
  if (transport_state != STATE_PLAY)
     { 
      PaStreamParameters outputParameters;

      outputParameters.device = pa_device_id_out;
      outputParameters.channelCount = documents->current->wave_edit->waveform->fb->channels;
      outputParameters.sampleFormat = paFloat32;
      outputParameters.sampleFormat |= paNonInterleaved;
      
      outputParameters.suggestedLatency = Pa_GetDeviceInfo (outputParameters.device)->defaultLowOutputLatency;;
      outputParameters.hostApiSpecificStreamInfo = NULL;

      PaStreamParameters inputParameters;

      inputParameters.device = pa_device_id_in;
      inputParameters.channelCount = documents->current->wave_edit->waveform->fb->channels;
      inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
      inputParameters.suggestedLatency = Pa_GetDeviceInfo (inputParameters.device)->defaultLowOutputLatency;;
      inputParameters.hostApiSpecificStreamInfo = NULL;

  
      PaError err = Pa_OpenStream (&pa_stream,
                                   NULL, //&inputParameters,
                                   &outputParameters,
 		                           documents->current->wave_edit->waveform->fb->samplerate,
		                           buffer_size_frames,
		                           paNoFlag,
		                           pa_stream_callback,
		                           NULL//&pe 
	                              ); 	
   
      qDebug() << Pa_GetErrorText (err);
       
      if (err < 0)
         {
          log->log (Pa_GetErrorText (err));
          
          QMessageBox::warning (0, tr ("Error!"), 
                               tr ("Something wrong!\n Try to select another sound port[s] at Tune - Sound options page, or release the sound device from another application use."));
          
          pa_stream = 0;
          return;
         }
     
      
      transport_play->setIcon (QIcon (":/icons/play.png"));
   
      if (d->wave_edit->waveform->play_looped && d->wave_edit->waveform->selected)
          //set the buffer offset and cursor to the start of the selection
          d->wave_edit->waveform->fb->offset = d->wave_edit->waveform->frames_start();
    
          
      wnd_fxrack->fx_rack->set_state_all (FXS_RUN);
      wnd_fxrack->fx_rack->reset_all_fx (d->wave_edit->waveform->fb->samplerate, d->wave_edit->waveform->fb->channels);
      d->wave_edit->waveform->timer.start();
      wnd_fxrack->level_meter->init_state = false;
      wnd_fxrack->tm_level_meter.start();
     
     
     ///////////////////////////////////////////
     
      
      if (dsp->temp_float_buffer)
          delete dsp->temp_float_buffer;
 
     dsp->temp_float_buffer = new CFloatBuffer (buffer_size_frames, d->wave_edit->waveform->fb->channels);
     //dsp->temp_float_buffer->allocate_interleaved();
 
 ////////////////////////////////////////////////////
         
      transport_state = STATE_PLAY;
      err = Pa_StartStream (pa_stream);
      qDebug() << Pa_GetErrorText (err);

       if (err < 0)
          {
           log->log (Pa_GetErrorText (err));
           pa_stream = 0;
          }
     
       return;
      }

     
  transport_state = STATE_PAUSE;
  
  wnd_fxrack->fx_rack->set_state_all (FXS_PAUSE);

  if (d)
     d->wave_edit->waveform->timer.stop();

  wnd_fxrack->tm_level_meter.stop();

  transport_play->setIcon (QIcon (":/icons/pause.png"));
}


void CEKO::stop_recording()
{
  if (pa_stream)
     {
      Pa_AbortStream (pa_stream_in);	
      Pa_CloseStream (pa_stream_in);	
      pa_stream_in = 0;
     }

   sf_close  (file_temp_handle);
       
   QString temppath = settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile;    
       
   if (! file_exists (temppath))
      {
        log->log (tr ("Cannot open the temporary file with the recorded sound."));
        return;
       } 
      
   CDocument *d = documents->open_file (temppath, false);
      
   int sndfile_format = 0;
   sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;
   
   int format = settings->value ("def_sndfile_format", sndfile_format).toInt();
 
   d->wave_edit->waveform->fb->sndfile_format = format;
 
   if (file_exists (settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile))
      QFile::remove (settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile);

   transport_state = STATE_STOP;
}


void CEKO::slot_transport_stop()
{
  wnd_fxrack->tm_level_meter.stop();
  wnd_fxrack->fx_rack->set_state_all (FXS_STOP);
  transport_play->setIcon (QIcon (":/icons/play.png"));
  
  if (transport_state == STATE_RECORD)
     {
      stop_recording();
      return;
     }
  
  transport_state = STATE_STOP;
  
  if (pa_stream)
     {
       Pa_CloseStream (pa_stream);	
       pa_stream = 0;
     }
  
  CDocument *d = documents->get_current(); 
  
  if (d)
     {
      d->wave_edit->waveform->timer.stop();
      d->wave_edit->waveform->fb->offset = 0;
      d->wave_edit->waveform->scrollbar->setValue (0);
     }
}


//пашет вроде
void CEKO::ed_copy_to_new()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return;
  
  if (! d->wave_edit->waveform->selected)
     return;
  
  d->wave_edit->waveform->copy_selected();
  
  if (! sound_clipboard)
     return;
  
  CDocument *new_document = documents->create_new();
  main_tab_widget->setCurrentIndex (idx_tab_edit);

  new_document->wave_edit->waveform->fb->copy_from (sound_clipboard); 
  
  new_document->wave_edit->waveform->recalc_view();
  new_document->wave_edit->waveform->prepare_image();
  
  new_document->wave_edit->waveform->init_state = false;
  new_document->wave_edit->timeruler->init_state = false;
  
  new_document->wave_edit->waveform->update();
}


//ВЫЛЕТ
void CEKO::ed_copy_to_new_fmt()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  if (! d->wave_edit->waveform->selected)
     return;

  d->wave_edit->waveform->copy_selected();

  if (! sound_clipboard)
     return;

  CDocument *new_document = documents->create_new();
  main_tab_widget->setCurrentIndex (idx_tab_edit);

  new_document->wave_edit->waveform->fb->copy_from_w_resample (sound_clipboard);

  new_document->wave_edit->waveform->recalc_view();
  new_document->wave_edit->waveform->prepare_image();

  new_document->wave_edit->waveform->init_state = false;
  new_document->wave_edit->timeruler->init_state = false;

  new_document->wave_edit->waveform->update();
}


void CEKO::ed_delete()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return;
  
  d->wave_edit->waveform->delete_selected();   
}


void CEKO::file_info()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return;
  
  int f = d->wave_edit->waveform->fb->sndfile_format & SF_FORMAT_TYPEMASK;
  QString format = file_formats->hformatnames.value (f);
  int st = d->wave_edit->waveform->fb->sndfile_format & SF_FORMAT_SUBMASK;
  QString subtype = file_formats->hsubtype.value (st);;
 
  log->log (tr ("samplerate: %1").arg (QString::number (d->wave_edit->waveform->fb->samplerate)));
  log->log (tr ("channels: %1").arg (QString::number (d->wave_edit->waveform->fb->channels)));
  log->log (subtype);
  log->log (format);

  log->log (d->file_name);
}


CChangeFormatWindow::CChangeFormatWindow (QWidget *parent, CWaveform *waveform, int fm): QDialog (parent)
{
  setModal (true);
  
  wf = waveform;
  
  if (wf)
     fmt = wf->fb->sndfile_format;
  else
      fmt = fm;   

  cmb_format = new QComboBox;
  cmb_subtype = new QComboBox;
  cmb_samplerate = new QComboBox;
  cmb_samplerate->setEditable (true);
  
  channels = new QSpinBox; 

  if (wf)
     channels->setValue (wf->fb->channels);

  cmb_format->addItems (file_formats->hformatnames.values());
  
  int f = (fmt & SF_FORMAT_TYPEMASK);
  QString sf = file_formats->hformatnames.value (f);
  cmb_format->setCurrentIndex (cmb_format->findText (sf));
  
  connect (cmb_format, SIGNAL(currentIndexChanged (const QString &)),
           this, SLOT(format_currentIndexChanged (const QString &)));

  format_currentIndexChanged (sf);
  
  QStringList sl_samplerates;
  sl_samplerates.append ("8000");
  sl_samplerates.append ("11025");
  sl_samplerates.append ("22050");
  sl_samplerates.append ("32000");
  sl_samplerates.append ("44100");
  sl_samplerates.append ("48000");
  sl_samplerates.append ("96000");
  sl_samplerates.append ("176400");
  sl_samplerates.append ("192000");
  
  cmb_samplerate->addItems (sl_samplerates);
  
  int i = 0;
  
  if (wf)
     i = cmb_samplerate->findText (QString::number (wf->fb->samplerate));
      
  if (i == -1)
     i = 0; 

  cmb_samplerate->setCurrentIndex (i);
  
  QVBoxLayout *lt_v = new QVBoxLayout;

  QHBoxLayout *lt_h = new QHBoxLayout;
  QLabel * l = new QLabel (tr ("Format"));
  lt_h->addWidget (l);
  lt_h->addWidget (cmb_format);
  lt_v->addLayout (lt_h);
  
  
  lt_h = new QHBoxLayout;
  l = new QLabel (tr ("Subtype"));
  lt_h->addWidget (l);
  lt_h->addWidget (cmb_subtype);
  lt_v->addLayout (lt_h);
  
  
  lt_h = new QHBoxLayout;
  l = new QLabel (tr ("Samplerate"));
  lt_h->addWidget (l);
  lt_h->addWidget (cmb_samplerate);
  lt_v->addLayout (lt_h);
  
  
  lt_h = new QHBoxLayout;
  l = new QLabel (tr ("Channels"));
  lt_h->addWidget (l);
  lt_h->addWidget (channels);
  lt_v->addLayout (lt_h);


  lt_h = new QHBoxLayout;
  
  QPushButton *bt_apply = new QPushButton (tr ("OK"));
  QPushButton *bt_exit = new QPushButton (tr ("Exit"));

  connect (bt_exit, SIGNAL(clicked()), this, SLOT(reject()));
  connect (bt_apply, SIGNAL(clicked()), this, SLOT(accept()));

  lt_h->addWidget (bt_apply);
  lt_h->addWidget (bt_exit);

  lt_v->addLayout (lt_h);

  setLayout (lt_v);
  setWindowTitle (tr ("Sound file format"));
}


void CChangeFormatWindow::format_currentIndexChanged (const QString &text)
{
  cmb_subtype->clear();
  
  int f = file_formats->hformatnames.key (text);
  
  QStringList sl;
  QList <int> values = file_formats->hformat.values (f);
  
  for (int i = 0; i < values.size(); ++i)
      {
       sl.append (file_formats->hsubtype.value (values.at(i))); 
      } 
  
  cmb_subtype->addItems (sl);
    
  int st = (fmt & SF_FORMAT_SUBMASK); 
  QString ssubtype = file_formats->hsubtype.value (st);
  
  int i = cmb_subtype->findText (ssubtype);
  if (i == -1)
     i = 0;
  
  cmb_subtype->setCurrentIndex (i);
}


void CEKO::file_change_format()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return;

  CChangeFormatWindow *w = new CChangeFormatWindow (0, d->wave_edit->waveform, 0);
  
  int result = w->exec();
  
  if (result != QDialog::Accepted)
     {
      delete w;
      return;
     }
  
  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);
  
  int f = file_formats->hformatnames.key (w->cmb_format->currentText());
  int stype = file_formats->hsubtype.key (w->cmb_subtype->currentText());
  
  d->wave_edit->waveform->fb->sndfile_format = 0;
  d->wave_edit->waveform->fb->sndfile_format = f | stype; 
  
  if (w->cmb_samplerate->currentText().toInt() != w->wf->fb->samplerate)
     {
      CFloatBuffer *b = d->wave_edit->waveform->fb->resample (w->cmb_samplerate->currentText().toInt());
      delete d->wave_edit->waveform->fb;
      d->wave_edit->waveform->fb = b;
      d->wave_edit->waveform->magic_update();
     }

  if (w->channels->value() != w->wf->fb->channels)
     {
      if (w->wf->fb->channels == 1)
         {
          CFloatBuffer *t = d->wave_edit->waveform->fb->convert_to_stereo (false);
          if (! t)
             return;
     
          delete d->wave_edit->waveform->fb;
          d->wave_edit->waveform->fb = t;
          d->wave_edit->waveform->magic_update();
         }
      else
          if (w->wf->fb->channels == 2)
            {
             CFloatBuffer *t = d->wave_edit->waveform->fb->convert_to_mono();
             if (! t)
                return;
     
             delete d->wave_edit->waveform->fb;
             d->wave_edit->waveform->fb = t;
             d->wave_edit->waveform->magic_update();
            }
     }
     
  delete w;
}
  
//ПАШЕТ
void CEKO::fn_stereo_to_mono()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  if (! d->wave_edit->waveform->fb)
     return; 

  if (d->wave_edit->waveform->fb->channels != 2)
     return;

  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);
  
  CFloatBuffer *t = d->wave_edit->waveform->fb->convert_to_mono();
  if (! t)
     return;

  delete d->wave_edit->waveform->fb;
  d->wave_edit->waveform->fb = t;
  
  d->wave_edit->waveform->magic_update();
}


void CEKO::fun_51_to_stereo (int algo)
{
  /*CDocument *d = documents->get_current();
  if (! d)
     return;

  if (! d->wave_edit->waveform->sound_buffer->buffer)
     return;

  if (d->wave_edit->waveform->sound_buffer->channels != 6)
     return;

  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);

  CSoundBuffer *t = d->wave_edit->waveform->sound_buffer->convert_ch6_to_stereo (algo);
  if (! t)
     return;

  delete d->wave_edit->waveform->sound_buffer;
  d->wave_edit->waveform->sound_buffer = t;

  d->wave_edit->waveform->magic_update();*/
}


void CEKO::fn_51_to_stereo()
{
  //fun_51_to_stereo (0);
}


void CEKO::fn_51_to_stereo_dlike()
{
  //fun_51_to_stereo (1);
}


//ПАШЕТ
void CEKO::fn_ch_mono_to_stereo (bool full) 
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 
  
  if (! d->wave_edit->waveform->fb)
     return; 

  if (d->wave_edit->waveform->fb->channels != 1)
     return;

  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);
  
  CFloatBuffer *t = d->wave_edit->waveform->fb->convert_to_stereo (full);
  if (! t)
     return;
     
  delete d->wave_edit->waveform->fb;
  d->wave_edit->waveform->fb = t;
  
  d->wave_edit->waveform->magic_update();
}


//ПАШЕТ
void CEKO::fn_mono_to_stereo_half()  
{
  fn_ch_mono_to_stereo (false);
}


//ПАШЕТ
void CEKO::fn_mono_to_stereo_full()  
{
  fn_ch_mono_to_stereo (true);
}


void CEKO::cb_show_meterbar_in_db_changed (int value)
{
  settings->setValue ("meterbar_show_db", value);
  documents->apply_settings();
}


void CEKO::fn_stat_rms()  
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  double sqr_sum = 0.0d;

  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();

//  QTime tm;
//  tm.start();
  
  for (size_t i = start; i < end; i++)
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
     {
      sqr_sum += (d->wave_edit->waveform->fb->buffer[ch][i] * 
                  d->wave_edit->waveform->fb->buffer[ch][i]);
     }
         
//  qDebug() << "tm:" << tm.elapsed();       
         
  size_t range = (end - start) * d->wave_edit->waveform->fb->channels;       
         
  double srms = sqrt (sqr_sum / range);
  float rms = 20.0f * log10 (srms / 1.0f);

  log->log (tr ("RMS is %1 dB").arg (rms));
}

//ПАШЕТ!
void CEKO::fn_fade_out()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 
 
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);
  
  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();

  size_t frames_per_step = (end - start) / 100;
  
  
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
      {
       int vol = 0;
  
       for (size_t i = start; i <= end; i++)
           {
           if (i % frames_per_step == 0)
              vol += 1.0f;
      
           d->wave_edit->waveform->fb->buffer[ch][i] -= 
                                  get_fvalue (d->wave_edit->waveform->fb->buffer[ch][i], vol);
          }
      }
    
  d->wave_edit->waveform->magic_update();
}  


//ПАШЕТ, но проверить как звучит!
void CEKO::fn_fade_in()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 
 
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);
 
  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();

  size_t frames_per_step = (end - start) / 100;
   
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
  {
   int vol = -100;

   for (size_t i = start; i <= end; i++)

      {
       if (i % frames_per_step == 0)
          vol += 1.0f;
      
       d->wave_edit->waveform->fb->buffer[ch][i] += get_fvalue (d->wave_edit->waveform->fb->buffer[ch][i],
                                                                vol);
             
      }
   }   
   
  d->wave_edit->waveform->magic_update();
}  


/*
void CEKO::fn_fade_in()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 
 
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);
  
  size_t frames_start = d->wave_edit->waveform->sample_start() / d->wave_edit->waveform->sound_buffer->channels;
  size_t frames_end = d->wave_edit->waveform->sample_end() / d->wave_edit->waveform->sound_buffer->channels;;

  CFloatLinearInterpolatorSimple lp (frames_start, -48, frames_end, 0);

  size_t sampleno = d->wave_edit->waveform->sample_start();

  for (size_t x = frames_start; x < frames_end; x++)
      {
       
       float y = lp.get_y_at_x (x);
       if (y == 0)
         continue;
       
       float v3 = db2lin (y);


       for (int ch = 0; ch < d->wave_edit->waveform->sound_buffer->channels; ch++) 
            d->wave_edit->waveform->sound_buffer->buffer[sampleno++] *= v3;
       }
  
  
  d->wave_edit->waveform->magic_update();
}  
*/

void CEKO::ed_deselect()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  d->wave_edit->waveform->deselect();
  d->wave_edit->waveform->update();
}


void CEKO::ed_select_all()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  d->wave_edit->waveform->select_all();
}


void CEKO::ed_trim()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 
 
  if (! d->wave_edit->waveform->selected)
     return;
 
  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);

 //copy selection to temp buffer
     
  CFloatBuffer *fb = d->wave_edit->waveform->fb->copy (d->wave_edit->waveform->frames_start(), 
                                                       d->wave_edit->waveform->frames_end() - d->wave_edit->waveform->frames_start());

  //delete source buffer
 
  delete d->wave_edit->waveform->fb;

 //replace it with temp buffer
  
  d->wave_edit->waveform->fb = fb;
  d->wave_edit->waveform->select_all();
  d->wave_edit->waveform->magic_update();
}


void CEKO::bt_set_def_format_clicked()
{
  int sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;

  int t = settings->value ("def_sndfile_format", sndfile_format).toInt();

  CChangeFormatWindow *w = new CChangeFormatWindow (0, 0, t);

  w->channels->setValue (settings->value ("def_channels", 1).toInt());

  int i = 0;
  int sr = settings->value ("def_samplerate", 44100).toInt();
  
  i = w->cmb_samplerate->findText (QString::number (sr));
      
  if (i == -1)
     i = 0; 

  w->cmb_samplerate->setCurrentIndex (i);
  
  int result = w->exec();
  
  if (result != QDialog::Accepted)
     {
      delete w;
      return;
     }
    
  int f = file_formats->hformatnames.key (w->cmb_format->currentText());
  int stype = file_formats->hsubtype.key (w->cmb_subtype->currentText());
  
  int ff = 0;
  ff = f | stype; 
      
  settings->setValue ("def_sndfile_format", ff);
  settings->setValue ("def_samplerate", w->cmb_samplerate->currentText().toInt());
  settings->setValue ("def_channels", w->channels->value());

  delete w;
}


void CEKO::spb_def_channels_valueChanged (int i)
{
  settings->setValue ("def_channels", i);
}


//ПАШЕТ
void CEKO::fn_norm()
{
  CDocument *d = documents->get_current();
  if (! d)
     return; 
  
  bool ok;
  double db = QInputDialog::getDouble(this, tr("Normalize to dB"),
                                       tr("dB:"), -3.0, -96.0, 0, 2, &ok);
  if (! ok)
     return; 
     
     
  /*
   
   float offset = (float) input_double_value (tr ("fix DC offset"), tr ("Offset in samples:"),
                                             -1.0d, 1.0d, 0.0d, 0.01d);
 
    
    
   */   
     
  float afactor = (float) pow (10.0, db / 20.0);
   
 
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);

  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();

  float max_peak = 0.0f;
  
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
  for (size_t i = start; i < end; i++)
      {
       if (float_less_than (max_peak, d->wave_edit->waveform->fb->buffer[ch][i]))
          max_peak = d->wave_edit->waveform->fb->buffer[ch][i]; 
      }      

  float scale = afactor / max_peak;

  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
  for (size_t i = start; i < end; i++)
      d->wave_edit->waveform->fb->buffer[ch][i] *= scale;
  
  d->wave_edit->waveform->magic_update();
}  


void CEKO::fn_reverse()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);
  
  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();
  
  size_t len = end - start;
   
        
  CFloatBuffer *fb = new CFloatBuffer (len, d->wave_edit->waveform->fb->channels);
    
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
     {
      size_t c = len;
 
      for (size_t  i = start; i < end; i++)
          fb->buffer[ch][--c] = d->wave_edit->waveform->fb->buffer[ch][i];
     }    
      
      
  d->wave_edit->waveform->fb->overwrite_at (fb, start);
  
  delete fb;
  
  d->wave_edit->waveform->magic_update();
}  


void CEKO::fn_swap_channels()  
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  if (d->wave_edit->waveform->fb->channels != 2)
     return;

  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);
  
  float *t = d->wave_edit->waveform->fb->buffer[0];
  d->wave_edit->waveform->fb->buffer[0] = d->wave_edit->waveform->fb->buffer[1];
  d->wave_edit->waveform->fb->buffer[1] = t;
  
  d->wave_edit->waveform->magic_update();
}

//изучить как оно работает если каналов больше двух
void CEKO::fn_copy_channel()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  bool ok;
  int channel = QInputDialog::getInt (this, "?",
                                      tr("Which channel?"), 1, 1, 2147483647, 1, &ok);
                                          
  if (! ok || channel < 0)
     return;
         
  if (channel > d->wave_edit->waveform->fb->channels)
     return;
     
     
  
  delete sound_clipboard;
  
  sound_clipboard = new CFloatBuffer (d->wave_edit->waveform->fb->length_frames, 1);
  memcpy (sound_clipboard->buffer[0], 
          d->wave_edit->waveform->fb->buffer[channel], 
          d->wave_edit->waveform->fb->length_frames * sizeof (float));
}


//FIXME works for mono and stereo only
void CEKO::fn_mute_channel()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  bool ok;
  int channel = QInputDialog::getInt (this, "?",
                                     tr ("Which channel?"), 1, 1, 2147483647, 1, &ok);

  if (! ok || channel < 1)
     return;

  if (channel > d->wave_edit->waveform->fb->channels)
     return;


  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);

  memset (d->wave_edit->waveform->fb->buffer[channel], 0, 
          d->wave_edit->waveform->fb->length_frames * sizeof (float));  
  
  d->wave_edit->waveform->magic_update();
}


void CEKO::spb_max_undos_valueChanged (int i)
{
  settings->setValue ("max_undos", i);
}


void CEKO::cb_play_looped_changed (int value)
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  d->wave_edit->waveform->play_looped = value;
}


void CEKO::nav_play_looped()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  d->wave_edit->waveform->play_looped = ! d->wave_edit->waveform->play_looped;
  cb_play_looped->setChecked (d->wave_edit->waveform->play_looped);
}


#define TWO_PI 2 * M_PI
//ПАШЕТ
void CEKO::generate_sine()
{
  QDialog w_sine_gen;
  w_sine_gen.setWindowTitle (tr ("Sine wave parameters"));
  
  QVBoxLayout *v_box = new QVBoxLayout;
  w_sine_gen.setLayout (v_box);
  w_sine_gen.setModal (true);
 
  QHBoxLayout *h_box = new QHBoxLayout;

  QLabel lf (tr ("Frequency"));  
  QSpinBox sb_f;  
  sb_f.setMaximum (9999999);
  sb_f.setValue (440);

  h_box->addWidget (&lf);
  h_box->addWidget (&sb_f);

  v_box->addLayout (h_box);

  h_box = new QHBoxLayout;

  QLabel ld (tr ("Duration in seconds"));  
  QSpinBox sp_duration;
  sp_duration.setMinimum (1);
  sp_duration.setValue (1);
  
  h_box->addWidget (&ld);
  h_box->addWidget (&sp_duration);

  v_box->addLayout (h_box);

  h_box = new QHBoxLayout;

  QLabel la (tr ("Amplitude in dB"));
  QDoubleSpinBox sp_amplitude;

  sp_amplitude.setMinimum (-128.0d);
  sp_amplitude.setSingleStep (0.1d);

  sp_amplitude.setMaximum (0.0d);
  sp_amplitude.setValue (-3.0d);


  h_box->addWidget (&la);
  h_box->addWidget (&sp_amplitude);

  v_box->addLayout (h_box);

  h_box = new QHBoxLayout;

  QPushButton *bt_apply = new QPushButton (tr ("OK"));
  QPushButton *bt_exit = new QPushButton (tr ("Cancel"));

  connect (bt_exit, SIGNAL(clicked()), &w_sine_gen, SLOT(reject()));
  connect (bt_apply, SIGNAL(clicked()), &w_sine_gen, SLOT(accept()));

  h_box->addWidget (bt_apply);
  h_box->addWidget (bt_exit);

  v_box->addLayout (h_box);    
  int result = w_sine_gen.exec();
  
  if (result != QDialog::Accepted)
      return;

  CDocument *new_document = documents->create_new();
  main_tab_widget->setCurrentIndex (idx_tab_edit);

  int frequency = sb_f.value();
  size_t len_seconds = sp_duration.value();

  float amplitude = db2lin((float) sp_amplitude.value()); //radius
  
  size_t sample_rate =  settings->value ("def_samplerate", 44100).toInt();
  size_t frames_count = len_seconds * sample_rate;

  //qDebug() << "frames_count = " << frames_count; 
  //qDebug() << "samples_count = " << samples_count; 

  delete new_document->wave_edit->waveform->fb;

  new_document->wave_edit->waveform->fb = new CFloatBuffer (frames_count, 
                                          settings->value ("def_channels", 1).toInt());
  
  
  new_document->wave_edit->waveform->fb->samplerate = sample_rate;
  
  float curphase = 0.0; // use M_PI/2 for cosine
  float incr = frequency * TWO_PI / new_document->wave_edit->waveform->fb->samplerate;
       
  qDebug() << "okkkkk";     
         
  for (size_t i = 0; i < frames_count; i++)
      {
       float s = (float) sin (curphase) * amplitude;

       curphase += incr;
  
       if (curphase >= TWO_PI)
           curphase -= TWO_PI;
   
       for (size_t ch = 0; ch < new_document->wave_edit->waveform->fb->channels; ch++)
           {
            new_document->wave_edit->waveform->fb->buffer[ch][i] = s;
           }
     }          
  
  new_document->wave_edit->waveform->recalc_view();
  new_document->wave_edit->waveform->prepare_image();
  
  new_document->wave_edit->waveform->init_state = false;
  new_document->wave_edit->timeruler->init_state = false;
  
  new_document->wave_edit->waveform->update();
}

//ПАШЕТ
void CEKO::fn_silence_selection()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 
  
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);
  
  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();

  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
       memset (d->wave_edit->waveform->fb->buffer[ch] + start, 0, (end - start) * sizeof (float));
    
  d->wave_edit->waveform->magic_update();
}

//ПАШЕТ
void CEKO::fn_insert_silence()
{
  CDocument *d = documents->get_current(); 
  if (! d)
     return; 

  bool ok;
  int mseconds = QInputDialog::getInt (this, tr ("?"),
                                             tr("How many milliseconds?"), 1000, 2, 2147483647, 1, &ok);
                                          
  if (! ok || mseconds <= 0)
     return;
  
  size_t frames_count = d->wave_edit->waveform->fb->samplerate * mseconds / 1000;
    
  d->wave_edit->waveform->undo_take_shot (UNDO_INSERT, d->wave_edit->waveform->fb->offset + frames_count/* - 1*/);
   
  CFloatBuffer *tfb = new CFloatBuffer (frames_count, d->wave_edit->waveform->fb->channels); 
  tfb->samplerate = d->wave_edit->waveform->fb->samplerate;
  //tfb->copy_params (d->wave_edit->waveform->fb);
         
  d->wave_edit->waveform->fb->paste_at (tfb, d->wave_edit->waveform->fb->offset);
                                                
  delete tfb;                        
                           
  d->wave_edit->waveform->deselect();
  d->wave_edit->waveform->magic_update();
}



//FIXME
void CEKO::file_import_raw()
{

/////////////////////

  QFileDialog dialog (this);
  QSize size = settings->value ("dialog_size", QSize (width(), height())).toSize();
  dialog.resize (size);

  dialog.setFilter (QDir::AllEntries | QDir::Hidden);

  QList<QUrl> sidebarUrls = dialog.sidebarUrls();
  QList<QUrl> sidebarUrls_old = dialog.sidebarUrls();

#ifdef Q_QS_X11

  QDir volDir("/mnt");
  QStringList volumes (volDir.entryList (volDir.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes)
          sidebarUrls.append (QUrl::fromLocalFile ("/mnt/" + v));

  QDir volDir2 ("/media");
  QStringList volumes2 (volDir2.entryList (volDir2.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes2)
          sidebarUrls.append (QUrl::fromLocalFile ("/media/" + v));

#endif

  dialog.setSidebarUrls (sidebarUrls);

  dialog.setFileMode (QFileDialog::ExistingFiles);
  dialog.setAcceptMode (QFileDialog::AcceptOpen);

  CDocument *d = documents->get_current();
  if (d)
     {
      if (file_exists (d->file_name))
          dialog.setDirectory (get_file_path (d->file_name));
      else
          dialog.setDirectory (dir_last);
     }
  else
      dialog.setDirectory (dir_last);

  
  if (! dialog.exec())
     {
      dialog.setSidebarUrls (sidebarUrls_old);
      return;
     } 

  dialog.setSidebarUrls (sidebarUrls_old);
  settings->setValue ("dialog_size", dialog.size());
///////////////////


  int sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_RAW | SF_FORMAT_PCM_16;

  int t = sndfile_format; //settings->value ("def_sndfile_format", sndfile_format).toInt();

  CChangeFormatWindow *w = new CChangeFormatWindow (0, 0, t);
  w->channels->setValue (1);

  int i = 0;
  int sr = settings->value ("def_samplerate", 44100).toInt();
  
  i = w->cmb_samplerate->findText (QString::number (sr));
      
  if (i == -1)
     i = 0; 

  w->cmb_samplerate->setCurrentIndex (i);
  
  int result = w->exec();
  
  if (result != QDialog::Accepted)
     {
      delete w;
      return;
     }
    
  int f = file_formats->hformatnames.key (w->cmb_format->currentText());
  int stype = file_formats->hsubtype.key (w->cmb_subtype->currentText());
  
  int raw_file_format = f | stype;    //new sound format
      
  int raw_srate = w->cmb_samplerate->currentText().toInt();

  SF_INFO sf;
  sf.format = raw_file_format;
  sf.samplerate = raw_srate;
  sf.channels = w->channels->value();

  delete w;

///////

  QStringList fileNames = dialog.selectedFiles();
  if (fileNames.count() == 0)
     return;

  QString fname = fileNames[0];
  
  SNDFILE *file = sf_open (fname.toLatin1(), SFM_READ, &sf);
  float *buffer = new float [sf.channels * sf.frames];
  sf_count_t zzz = sf_readf_float (file, buffer, sf.frames);
  
  sf_close (file);

 // qDebug() << "zzz = " << zzz;

  if (zzz == 0)
     {
      delete [] buffer;   
      return;
     }
  
  CFloatBuffer *tfb = new CFloatBuffer (buffer, sf.frames, sf.channels);
    
  tfb->samplerate = sf.samplerate;
  tfb->sndfile_format = raw_file_format;
  
  delete [] buffer;

  CDocument *new_document = documents->create_new();
  main_tab_widget->setCurrentIndex (idx_tab_edit);

  new_document->wave_edit->waveform->fb = tfb;
      
  new_document->wave_edit->waveform->recalc_view();
  
  new_document->wave_edit->waveform->prepare_image();
  
  new_document->wave_edit->waveform->init_state = false;
  new_document->wave_edit->timeruler->init_state = false;
  
  new_document->wave_edit->waveform->update();
}


void CEKO::spb_ogg_q_valueChanged (double d)
{
  settings->setValue ("ogg_q", d);
  ogg_q = d;
}


void CEKO::fn_detect_average_value()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end()/* - 1*/;

 // size_t i = start;

  float minpeak = 0.0f;
  float maxpeak = 0.0f;
  float accum = 0.0f;
  float t = 0.0f;

  for (size_t i = start; i < end; i++) 
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
      {
       t = fabs (d->wave_edit->waveform->fb->buffer[ch][i]);
       accum += t;
     
       if (! float_equal (t, 0.0f))
          {
           maxpeak = std::max ((float)maxpeak, (float)t);

           if (float_equal (minpeak, 0.0f))
              minpeak = t;
           else
               minpeak = std::min ((float)minpeak, (float)t);
           }
      }

  QString maxp;
  maxp.setNum (maxpeak);

  QString minp;
  minp.setNum (minpeak);
  
  QString dr;
  dr.setNum ((float)minpeak / maxpeak);

  float average = (float) accum / (float)(end - start);
  
  log->log (tr ("average level in samples = %1").arg (QString::number (average)));
  log->log (tr ("average level in dB = %1").arg (QString::number (float2db (average))));
  log->log (tr ("max level in samples = %1").arg (maxp));
  log->log (tr ("max level in dB = %1").arg (QString::number (float2db (maxpeak))));
  log->log (tr ("min level in samples = %1").arg (minp));
  log->log (tr ("min level in dB = %1").arg (QString::number (float2db (minpeak))));
  
 // log->log (tr ("Dynamic range = %1").arg (dr));
}


void CEKO::fn_dc_offset_detect()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();
  
  float accum = 0.0f;

  for (size_t i = start; i < end; i++) 
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
       {
        accum += d->wave_edit->waveform->fb->buffer[ch][i];
       }

  float average = (float) accum / (float)(end - start);

  log->log (tr ("DC offset in samples = %1").arg (QString::number (average)));
}


void CEKO::fn_dc_offset_fix_manually()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  float offset = (float) input_double_value (tr ("fix DC offset"), tr ("Offset in samples:"),
                                             -1.0d, 1.0d, 0.0d, 0.01d);

  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);

  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();
  
  for (size_t i = start; i < end; i++) 
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
        {
         d->wave_edit->waveform->fb->buffer[ch][i] -= offset;
        }

  d->wave_edit->waveform->magic_update();
}


void CEKO::fn_dc_offset_fix_auto()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);

  size_t start = d->wave_edit->waveform->frames_start();
  size_t end = d->wave_edit->waveform->frames_end();

  float accum = 0.0f;

  for (size_t i = start; i < end; i++) 
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
      {
       accum += (d->wave_edit->waveform->fb->buffer[ch][i]);
      }

  float average = (float) accum / (float)(end - start);

  for (size_t i = start; i < end; i++) 
  for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++)
      {
       d->wave_edit->waveform->fb->buffer[ch][i] -= average;
      }


  d->wave_edit->waveform->magic_update();
}

//OK
void CEKO::generate_noise()
{
  QDialog w_noise_gen;
  w_noise_gen.setWindowTitle (tr ("Noise parameters"));

  QVBoxLayout *v_box = new QVBoxLayout;
  w_noise_gen.setLayout (v_box);
  w_noise_gen.setModal (true);

  QHBoxLayout *h_box = new QHBoxLayout;

  QLabel ltype (tr ("Type"));
  QComboBox cbm_type;
  cbm_type.addItem (tr ("White"));
  cbm_type.addItem (tr ("Pink"));
  //cbm_type.addItem (tr ("Brown"));

  h_box->addWidget (&ltype);
  h_box->addWidget (&cbm_type);

  v_box->addLayout (h_box);

  h_box = new QHBoxLayout;

  QLabel ld (tr ("Duration in seconds"));
  QSpinBox sp_duration;
  sp_duration.setMinimum (1);
  sp_duration.setValue (1);

  h_box->addWidget (&ld);
  h_box->addWidget (&sp_duration);

  v_box->addLayout (h_box);

  h_box = new QHBoxLayout;

  QLabel la (tr ("Amplitude in dB"));
  QDoubleSpinBox sp_amplitude;

  sp_amplitude.setMinimum (-128.0d);
  sp_amplitude.setSingleStep (0.1d);

  sp_amplitude.setMaximum (0.0d);
  sp_amplitude.setValue (-3.0d);


  h_box->addWidget (&la);
  h_box->addWidget (&sp_amplitude);

  v_box->addLayout (h_box);

  h_box = new QHBoxLayout;

  QPushButton *bt_apply = new QPushButton (tr ("OK"));
  QPushButton *bt_exit = new QPushButton (tr ("Cancel"));

  connect (bt_exit, SIGNAL(clicked()), &w_noise_gen, SLOT(reject()));
  connect (bt_apply, SIGNAL(clicked()), &w_noise_gen, SLOT(accept()));

  h_box->addWidget (bt_apply);
  h_box->addWidget (bt_exit);

  v_box->addLayout (h_box);

  int result = w_noise_gen.exec();

  if (result != QDialog::Accepted)
      return;

  CDocument *new_document = documents->create_new();
  main_tab_widget->setCurrentIndex (idx_tab_edit);

  int ntype = cbm_type.currentIndex();
  size_t len_seconds = sp_duration.value();

  float amplitude = db2lin((float) sp_amplitude.value());

  size_t frames_count = len_seconds * new_document->wave_edit->waveform->fb->samplerate;


  CFloatBuffer *tfb = new CFloatBuffer (frames_count, settings->value ("def_channels", 1).toInt());
  tfb->samplerate = new_document->wave_edit->waveform->fb->samplerate;
 
  for (size_t ch = 0; ch < tfb->channels; ch++)  
     {
      float *buf;
      
      if (ntype == 0)
         buf = noise_generate_white (frames_count, amplitude);
      else
      if (ntype == 1)
         buf = noise_generate_pink2 (frames_count, amplitude);
      
      memcpy (tfb->buffer[ch], buf, frames_count * sizeof (float));
      delete [] buf;
     }

  delete new_document->wave_edit->waveform->fb;

  new_document->wave_edit->waveform->fb = tfb;

  new_document->wave_edit->waveform->recalc_view();
  new_document->wave_edit->waveform->prepare_image();

  new_document->wave_edit->waveform->init_state = false;
  new_document->wave_edit->timeruler->init_state = false;

  new_document->wave_edit->waveform->update();
}


void CEKO::cmb_icon_sizes_currentIndexChanged (const QString &text)
{
  settings->setValue ("icon_size", text);

  setIconSize (QSize (text.toInt(), text.toInt()));
  tb_fman_dir->setIconSize (QSize (text.toInt(), text.toInt()));
}


void CEKO::file_record()
{
  transport_state = STATE_RECORD;
  
  if (pa_stream)
     {
       Pa_CloseStream (pa_stream);	
       pa_stream = 0;
     }

  if (pa_stream_in)
     {
      Pa_AbortStream (pa_stream_in);	
      Pa_CloseStream (pa_stream_in);	
      pa_stream_in = 0;
     }


  fb_record->length_frames = buffer_size_frames;
  fb_record->allocate_interleaved();
  fb_record->settozero();
  
  int sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;

  int channels = settings->value ("def_channels", 1).toInt();
  int samplerate = settings->value ("def_samplerate", 44100).toInt();

  rec_channels = channels;

  SF_INFO sf;

  sf.samplerate = samplerate;
  sf.channels = channels;
  sf.format = sndfile_format;
 
  if (! sf_format_check (&sf))
     {
      qDebug() << "! sf_format_check (&sf)";
      transport_state = STATE_STOP;
      return;  
     }    
  
  QString tf = settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile;
  
  file_temp_handle = sf_open (tf.toUtf8().data(), SFM_WRITE, &sf);

  PaStreamParameters outputParameters;

  outputParameters.device = pa_device_id_out;
  outputParameters.channelCount = 2;
  outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
      outputParameters.suggestedLatency = Pa_GetDeviceInfo (outputParameters.device)->defaultLowOutputLatency;;
      outputParameters.hostApiSpecificStreamInfo = NULL;


      PaStreamParameters inputParameters;

      inputParameters.device = pa_device_id_in;
      inputParameters.channelCount = 2;
      inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
      inputParameters.suggestedLatency = Pa_GetDeviceInfo (inputParameters.device)->defaultLowOutputLatency;;
      inputParameters.hostApiSpecificStreamInfo = NULL;

  
      PaError err =  Pa_OpenStream 	(&pa_stream_in,
		                         &inputParameters,
		                         //NULL,
                                 &outputParameters,
 		                         samplerate,
		                         buffer_size_frames,
		                          paClipOff | paDitherOff,//paNoFlag,
		                         pa_input_stream_callback,
		                         NULL//&pe 
	                           ); 	
   
       qDebug() << Pa_GetErrorText (err);
       
       if (err < 0)
          {
           log->log (Pa_GetErrorText (err));
           pa_stream_in = 0;
           transport_state = STATE_STOP;
           return;
          }
     

       err = Pa_StartStream (pa_stream_in);
       qDebug() << Pa_GetErrorText (err);

       if (err < 0)
          {
           log->log (Pa_GetErrorText (err));
           pa_stream_in = 0;
           transport_state = STATE_STOP;
           return;
          }
      
       wnd_fxrack->level_meter->init_state = false;
       wnd_fxrack->tm_level_meter.start();
  
  log->log (tr("<b>Press Stop to stop recording!</b>"));      
   //    transport_play->setIcon (QIcon (":/icons/play.png"));

}




void CEKO::update_stylesheet (const QString &f)
{

//Update paletted

  int darker_val = settings->value ("darker_val", 100).toInt();


  QFontInfo fi = QFontInfo (qApp->font());

  QString fontsize = "font-size:" + settings->value ("app_font_size", fi.pointSize()).toString() + "pt;";  
  QString fontfamily = "font-family:" + settings->value ("app_font_name", qApp->font().family()).toString() + ";";  
  QString edfontsize = "font-size:" + settings->value ("editor_font_size", "16").toString() + "pt;";  
  QString edfontfamily = "font-family:" + settings->value ("editor_font_name", "Serif").toString() + ";";  
 
  QString stylesheet; 
  
  stylesheet = "QWidget, QWidget * {" + fontfamily + fontsize + "}\n";
  
  stylesheet += "CLogMemo, QTextEdit, QPlainTextEdit, QPlainTextEdit * {" + edfontfamily + fontsize + "}\n";
  
 
//Update themed

  QString cssfile = qstring_load (f);

  QString css_path = get_file_path (f) + "/"; 

  cssfile = cssfile.replace ("./", css_path); 

  cssfile += stylesheet;

  qApp->setStyleSheet ("");
  qApp->setStyleSheet (cssfile);
}


void CEKO::view_use_theme()
{
  QAction *a = qobject_cast<QAction *>(sender());
  QString css_fname = a->data().toString() + "/" + "stylesheet.css";
  
  if (! file_exists (css_fname))
     {
      log->log (tr ("There is no plugin file"));
      return;
    }

  update_stylesheet (css_fname);
  fname_stylesheet = css_fname;
  settings->setValue ("fname_stylesheet", fname_stylesheet);
}



bool has_css_file (const QString &path)
{
  QDir d (path);
  QStringList l = d.entryList();
  
  for (int i = 0; i < l.size(); i++)
     {
      if (l[i].endsWith(".css"))
          return true;
     }
 
  return false;
}

//uses dir name as menuitem, no recursion
void create_menu_from_themes (QObject *handler,
                               QMenu *menu,
                               const QString &dir,
                               const char *method
                               )
{
  menu->setTearOffEnabled (true);
  QDir d (dir);
  QFileInfoList lst_fi = d.entryInfoList (QDir::NoDotAndDotDot | QDir::Dirs,
                                          QDir::IgnoreCase | QDir::LocaleAware | QDir::Name);

  
  foreach (QFileInfo fi, lst_fi)
         {
          if (fi.isDir())
             {
              if (has_css_file (fi.absoluteFilePath()))   
                 {
                  QAction *act = new QAction (fi.fileName(), menu->parentWidget());
                  act->setData (fi.filePath());
                  handler->connect (act, SIGNAL(triggered()), handler, method);
                  menu->addAction (act);
                }
             else
                 {
                  QMenu *mni_temp = menu->addMenu (fi.fileName());
                  create_menu_from_themes (handler, mni_temp,
                                           fi.filePath(), method);

                 }
             }
         }
}


void CEKO::update_themes()
{
  menu_view_themes->clear();
  create_menu_from_themes (this,
                            menu_view_themes,
                            ":/themes",
                            SLOT (view_use_theme())
                            );
 
  
  create_menu_from_themes (this,
                            menu_view_themes,
                            dir_themes,
                            SLOT (view_use_theme())
                            );
}


void CEKO::cb_altmenu_stateChanged (int state)
{
  if (state == Qt::Unchecked)
      b_altmenu = false;
  else 
       b_altmenu = true;
  
  settings->setValue ("b_altmenu", b_altmenu);
}


MyProxyStyle::MyProxyStyle (QStyle * style): QProxyStyle (style)
{
     
}


void CEKO::file_export_mp3()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  QFileDialog dialog (this);
  QSize size = settings->value ("dialog_size", QSize (width(), height())).toSize();
  dialog.resize (size);

  dialog.setFilter(QDir::AllEntries | QDir::Hidden);
  dialog.setOption (QFileDialog::DontUseNativeDialog, true);


  QList<QUrl> sidebarUrls = dialog.sidebarUrls();
  QList<QUrl> sidebarUrls_old = dialog.sidebarUrls();

  sidebarUrls.append(QUrl::fromLocalFile(dir_sessions));

#ifdef Q_QS_X11

  QDir volDir("/mnt");
  QStringList volumes (volDir.entryList (volDir.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes)
          sidebarUrls.append (QUrl::fromLocalFile ("/mnt/" + v));

  QDir volDir2 ("/media");
  QStringList volumes2 (volDir2.entryList (volDir2.filter() | QDir::NoDotAndDotDot));

  foreach (QString v, volumes2)
          sidebarUrls.append (QUrl::fromLocalFile ("/media/" + v));

#endif

  dialog.setSidebarUrls(sidebarUrls);

  dialog.setFileMode (QFileDialog::AnyFile);
  dialog.setAcceptMode (QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite (false);
  dialog.setDirectory (dir_last);

  if (dialog.exec())
     {
      dialog.setSidebarUrls (sidebarUrls_old);

      QString fileName = dialog.selectedFiles().at(0);

      if (file_exists (fileName))
         {
          int ret = QMessageBox::warning (this, "EKO",
                                          tr ("%1 already exists\n"
                                          "Do you want to overwrite?")
                                           .arg (fileName),
                                          QMessageBox::Yes | QMessageBox::Default,
                                          QMessageBox::Cancel | QMessageBox::Escape);

          if (ret == QMessageBox::Cancel)
             return;
         }

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)

      QString command = settings->value ("mp3_encode", "lame.exe -b 320 -q 0 \"@FILEIN\" \"@FILEOUT\"").toString();
      command = QCoreApplication::applicationDirPath() + "/" + command;

#else

      QString command = settings->value ("mp3_encode", "lame -b 320 -q 0 \"@FILEIN\" \"@FILEOUT\"").toString();

#endif

      QString fname_input = settings->value ("temp_path", QDir::tempPath()).toString() + fname_tempfile;
  
      QString fname_output = fileName;
      if (! fileName.endsWith (".mp3", Qt::CaseInsensitive))
         fname_output = change_file_ext (fname_output, "mp3");
  
      command = command.replace ("@FILEIN", fname_input);
      command = command.replace ("@FILEOUT", fname_output);
      
      qDebug() << "fname_input = " << fname_input;
      qDebug() << "fname_output = " << fname_output;

      qDebug() << command;
      
      CTioPlainAudio tio_wav;

   //   tio_wav.format = SF_FORMAT_WAV;
//      tio_wav.format = tio_wav.format | SF_FORMAT_PCM_16;
      
//      tio_wav.samplerate = d->wave_edit->waveform->sound_buffer->samplerate;
      
      tio_wav.float_input_buffer = d->wave_edit->waveform->fb;
      
    
      tio_wav.save_16bit_pcm (fname_input);
      

      int exit_code = QProcess::execute (command);
       qDebug() << exit_code;
  
      if (exit_code < 0)
         {
          qDebug() << "cannot encode to MP3";
          return;
          
         } 
     }

  settings->setValue ("dialog_size", dialog.size());
  return;
}


void CEKO::cmb_buffer_size_frames_currentIndexChanged (const QString &text)
{
  settings->setValue ("buffer_size_frames", text);
  buffer_size_frames = text.toInt();  
}
  
//ПАШЕТ!
void CEKO::fn_delete_vol_envelope()  
{
  CDocument *d = documents->get_current();
  if (! d)
     return; 
     
  d->wave_edit->waveform->env_vol.clear();     
  d->wave_edit->waveform->magic_update();
}


//ПАШЕТ!
void CEKO::fn_apply_vol_envelope()  
{
  CDocument *d = documents->get_current();
  if (! d)
     return; 
 
  if (d->wave_edit->waveform->env_vol.points.size() == 0)
     return;
  
  d->wave_edit->waveform->undo_take_shot (UNDO_WHOLE);

 // QTime tm;
  //tm.start();

  for (int i = 0; i < d->wave_edit->waveform->env_vol.points.size() - 1; i++)
      {
       size_t x1 = d->wave_edit->waveform->env_vol.points[i]->position_frames;
       float y1 = d->wave_edit->waveform->env_vol.points[i]->value;
       
       size_t x2 = d->wave_edit->waveform->env_vol.points[i + 1]->position_frames;
       float y2 = d->wave_edit->waveform->env_vol.points[i + 1]->value;

       CFloatInterpolator *lip = new CFloatInterpolatorSimple (x1, y1, x2, y2);
    
       for (size_t x = x1; x < x2; x++)
           {
            float y = lip->get_y_at_x (x);
            
            if (y == 0)
               continue;
       
            float v1 = conv (y, 50, 100);
            float v2 = conv_to_db (v1, -50, 50, -90.0f, 6.0f);
            float v3 = db2lin (v2);

            for (size_t ch = 0; ch < d->wave_edit->waveform->fb->channels; ch++) 
                d->wave_edit->waveform->fb->buffer[ch][x] *= v3;
           }

       delete lip;   
      }
  

  
//  qDebug() << "tm:" << tm.elapsed();
  
  d->wave_edit->waveform->magic_update();
}  


#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
    
bool is_app_installed (const QString &binname)
{
  QString path = QCoreApplication::applicationDirPath() + "/" + binname;
  
  return file_exists (path);
}

#else

bool is_app_installed (const QString &binname)
{
  int status = QProcess::execute ("which " + binname);
  
  return ! status;
}


#endif


void CEKO::help_system_check()  
{

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
  
  if (! is_app_installed ("ffmpeg.exe"))
     {
      log->log (tr ("FFMPEG is not installed"));
     }
  else
      log->log (tr ("FFMPEG is installed, video files and MP3 import: enabled"));
   

  if (! is_app_installed ("mplayer.exe"))
     {
      log->log (tr ("Mplayer is not installed"));
     }
  else
      log->log (tr ("Mplayer is installed, video files and MP3 import: enabled"));
  
#else


 if (! is_app_installed ("ffmpeg"))
     {
      log->log (tr ("FFMPEG is not installed"));
     }
  else
      log->log (tr ("FFMPEG is installed, video files and MP3 import: enabled"));
   

  if (! is_app_installed ("mplayer"))
     {
      log->log (tr ("Mplayer is not installed"));
     }
  else
      log->log (tr ("Mplayer is installed, video files and MP3 import: enabled"));
  
#endif

  CDocument *d = documents->get_current();
  if (d)
     {
      if (pa_stream)
         {
          const PaStreamInfo *psi = Pa_GetStreamInfo (pa_stream); 	
          if (psi)
              {
               log->log (tr ("Input latency: %1 ms").arg (psi->inputLatency));
               log->log (tr ("Output latency: %1 ms").arg (psi->outputLatency));
              } 
         }
     }
     
     
  if (pa_stream_in)
     {
      const PaStreamInfo *psi = Pa_GetStreamInfo (pa_stream_in); 	
      if (psi)
         {
          log->log (tr ("Input latency: %1 ms").arg (psi->inputLatency));
          log->log (tr ("Output latency: %1 ms").arg (psi->outputLatency));
         } 
     }
}






void CEKO::test()
{
  CFloatBuffer *fb = new CFloatBuffer (12, 2);
  
  fb->to_interleaved();
  
  delete fb;
 
}



void CEKO::apply_fx_clicked()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

//  dsp->process_whole_document (d);
  
    if (! d)
     return;

//here we work with short buffer to process it and output to it
  
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);

  wnd_fxrack->fx_rack->set_state_all (FXS_RUN);
  wnd_fxrack->fx_rack->reset_all_fx (d->wave_edit->waveform->fb->samplerate, d->wave_edit->waveform->fb->channels);
      
  size_t frames_start = d->wave_edit->waveform->frames_start();
  size_t frames_end = d->wave_edit->waveform->frames_end();
  
  d->wave_edit->waveform->fb->pbuffer_reset();

  ////////////call fx chain

  size_t portion_size = buffer_size_frames;

  d->wave_edit->waveform->fb->pbuffer_inc (frames_start); 

  while (d->wave_edit->waveform->fb->offset < frames_end)
        {
         size_t diff = frames_end - d->wave_edit->waveform->fb->offset;
         
         if (diff < buffer_size_frames)     
            portion_size = diff;

         for (int i = 0; i < wnd_fxrack->fx_rack->effects.count(); i++)
             {
              if (! wnd_fxrack->fx_rack->effects[i]->bypass)
                 {
                  wnd_fxrack->fx_rack->effects[i]->realtime = false;
                 
                  wnd_fxrack->fx_rack->effects[i]->channels = d->wave_edit->waveform->fb->channels;
                  wnd_fxrack->fx_rack->effects[i]->samplerate = d->wave_edit->waveform->fb->samplerate;
                  
                  wnd_fxrack->fx_rack->effects[i]->execute (d->wave_edit->waveform->fb->pbuffer,
                                                            d->wave_edit->waveform->fb->pbuffer, 
                                                            portion_size);
                     
                 }
             }

         d->wave_edit->waveform->fb->pbuffer_inc (portion_size);  
        }


  for (int i = 0; i < wnd_fxrack->fx_rack->effects.count(); i++)
      {
       if (! wnd_fxrack->fx_rack->effects[i]->bypass)
           wnd_fxrack->fx_rack->effects[i]->bypass = true;
      }


  d->wave_edit->waveform->fb->offset = d->wave_edit->waveform->frames_start();
  d->wave_edit->waveform->magic_update();
 
  wnd_fxrack->fx_rack->bypass_all();
}
