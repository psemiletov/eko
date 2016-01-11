/***************************************************************************
 *   2010-2016 by Peter Semiletov                            *
 *   tea@list.ru                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   aint with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef DOCUMENT_H
#define DOCUMENT_H


#include <QObject>

#include <QHash>
#include <QLabel>
#include <QStatusBar>
#include <QMainWindow>
#include <QScrollBar>
#include <QCheckBox>
#include <QTimer>
#include <QPushButton>

#include <portaudio.h>


#include "logmemo.h"
#include "tio.h"
#include "floatbuffer.h"
#include "envelope.h"
#include "cvlevelmeter.h"
#include "fxrack.h"



enum ETransportStates
{
  STATE_STOP = 0,
  STATE_PLAY,
  STATE_PAUSE,
  STATE_EXIT,
  STATE_RECORD
};


enum EUndoModes
{
  UNDO_UNDEFINED = 0,
  UNDO_DELETE,
  UNDO_PASTE,
  UNDO_MODIFY,
  UNDO_WHOLE,
  UNDO_INSERT
};


class CDocumentHolder;
class CDocument;


class CTransportControl: public QObject
{
Q_OBJECT

signals:

  void play_pause();
  void stop();

public:

  void call_play_pause();
  void call_stop();
};


class CUndoElement: public QObject
{
  Q_OBJECT

public:
  
  int type;
  bool selected;
    
  size_t frames_per_section;

  size_t start_frames; //selection start
  size_t end_frames; //selection end
  
  size_t cursor_frames;

  CFloatBuffer *fb;
  
  CUndoElement();
  ~CUndoElement();
};


class CChannelSamples
{
public:

  float *samples;
  float temp;
  
  CChannelSamples (size_t size);
  ~CChannelSamples(); 
};


class CChannelMinmax
{
public:

  CChannelSamples *min_values;
  CChannelSamples *max_values;
      
  CChannelMinmax (size_t size);
  ~CChannelMinmax(); 
};


class CMinmaxes
{
public:

  CChannelMinmax **values;
    
  size_t count;
  
  CMinmaxes (size_t size, size_t sections);
  ~CMinmaxes(); 
};


class CWaveform;


class CTimeRuler: public QWidget
{
  Q_OBJECT

public:

  CWaveform *waveform;
  bool init_state;

  QColor background_color;
  QColor foreground_color;

  CTimeRuler (QWidget *parent);

protected:

  void paintEvent (QPaintEvent *event);
};


class CWaveEdit;


class CWaveform: public QWidget
{
  Q_OBJECT

public:

  //top-level link:
  CWaveEdit *wave_edit;
  

  CFloatBuffer *fb;
  CTimeRuler *timeruler;


  CEnvelope env_vol;

  bool show_db; //false if floats
  bool play_looped;

  int selection_selected; //0 - non, 1 - left, 2 - right
  int envelope_selected; //-1 - non
  
   
  CMinmaxes *minmaxes;   
  QScrollBar *scrollbar;

  QTimer timer;

  bool init_state;
  bool draw_shadow;

  bool mouse_pressed;
  bool normal_cursor_shape;
  bool selected;

  QImage waveform_image;

  float scale_factor;

  
  int previous_mouse_pos_x;
  
  
  size_t sel_start_frames;
  size_t sel_end_frames;
  
 //calculates positions:
  
  int get_cursor_position_sections(); //in sections
  int get_selection_start_sections(); //in sections
  int get_selection_end_sections();  //in sections

  void set_cursor_by_section (size_t section);

  size_t frames_per_section;

  size_t section_from;
  size_t section_to;
  size_t sections_total;

//colors

  QColor cl_waveform_background;
  QColor cl_waveform_foreground; 
  QColor cl_waveform_selection_foreground; 
  QColor cl_axis; 
  QColor cl_cursor; 
  QColor cl_text; 
  QColor cl_envelope; 
  QColor cl_env_point_selected; 
  QColor cl_shadow; 

  int waveform_selection_alpha;

  void load_color (const QString &fname);

//end of colors

  
  CWaveform (QWidget *parent);
  ~CWaveform();

  void set_cursor_value (size_t section);
  void set_selstart_value (size_t section);
  void set_selend_value (size_t section);

  void set_statusbar_text();
  void set_cursorpos_text();

  void scale (int delta);

  void recalc_view();
  void prepare_image();

  void magic_update();
  void fix_selection_bounds();
  
  void deselect();
  void select_all();
  
  void copy_selected();
  void delete_selected();
  void cut_selected();
    
  void paste();
  
  size_t frames_start();
  size_t frames_end();   

  void set_cursor_to_frames (size_t frame);

//undo/redo stuff
  QList <CUndoElement*> undos;
  int max_undos;
//  int current_undo;
 
  void undo_take_shot (int type, int param = 0); 
  void undo_top(); //simple test of the undo

  void flush_undos();

  void redo();
  
  
protected:

//  void dragEnterEvent (QDragEnterEvent *event);
//  void dropEvent (QDropEvent *event);
  void paintEvent(QPaintEvent *event);
  void mousePressEvent (QMouseEvent *event);  
  void wheelEvent (QWheelEvent *event );  
  void keyPressEvent (QKeyEvent *event);    
  void resizeEvent(QResizeEvent *event);
  
  void mouseMoveEvent (QMouseEvent *event); 
  void mouseReleaseEvent (QMouseEvent * event);  
  
  void mouseDoubleClickEvent ( QMouseEvent * event);
  
public slots:

  void timer_timeout();  
  
};


class CWaveEdit: public QWidget
{
  Q_OBJECT

public:

  CDocument *doc;
  CWaveform *waveform;
  
  QScrollBar *scb_horizontal;
 
  CTimeRuler *timeruler;

  CWaveEdit (QWidget *parent = 0);
  ~CWaveEdit();

  bool isReadOnly();
  
protected:

  void dragEnterEvent (QDragEnterEvent *event);
  void dropEvent (QDropEvent *event);
  
public slots:

  void scb_horizontal_valueChanged (int value);  
  
};


class CDocument: public QObject
{
  Q_OBJECT

public:

  QHash <QString, QString> fnameswoexts;
    
  CDocumentHolder *holder;
  

  CWaveEdit *wave_edit;

  QString file_name;
  QWidget *tab_page;
  
  size_t position;

  bool ronly;

  CDocument (QObject *parent = 0);
  ~CDocument();

  void paste();
  
  void create_new();
  
  void set_tab_caption (const QString &fileName);
  
  bool save_with_name (const QString &fileName);
  bool save_with_name_plain (const QString &fileName);
  
  bool open_file (const QString &fileName, bool set_fname = true);
  void reload();
  int get_tab_idx();
  QString get_triplex();
  void update_title (bool fullname = true);

  void effects_state_save();
  void effects_state_restore();
  void effects_close_all();
  void effects_update_params();
};


class CDocumentHolder: public QObject
{
  Q_OBJECT

public:

  CTioHandler tio_handler;

  QString fname_current_session;

  QHash <QString, QString> palette;

  QLabel *l_status_bar;
  QLabel *l_maintime; 
  
  CTransportControl *transport_control;  


  CDocument *current;
  
  QString dir_config;
  
  QString def_palette;

  QStatusBar *status_bar;
  CLogMemo *log;
  QList <CDocument*> list;
  QMainWindow *parent_wnd;
  QTabWidget *tab_widget;

  QMenu *recent_menu;
  QStringList recent_files;
  QString recent_list_fname;

  CDocumentHolder (QObject *parent = 0);
  ~CDocumentHolder();

  CDocument* create_new();
  CDocument* open_file (const QString &fileName, bool set_fname = true);
  void reload_recent_list();

  void save_to_session (const QString &fileName);
  void load_from_session (const QString &fileName);
  void load_palette (const QString &fileName);

  void close_current();
  void apply_settings();
  void apply_settings_single (CDocument *d);

  void add_to_recent (CDocument *d);
  void update_recent_menu();

  CDocument* get_current();

  public slots:

  void open_recent();
};


class CDSP: public QObject
{

Q_OBJECT

public:
 
  float maxl; //for the current nframes
  float maxr;

//mixer stuff
  float gain; //in dB
 
  CFloatBuffer *temp_float_buffer;

  CDSP (QObject *parent = 0);
  ~CDSP();
  
//  size_t process (CFloatBuffer *fb, size_t nframes);
  size_t process (CDocument *d, size_t nframes);

  size_t process_rec (float **buffer, size_t channels, size_t nframes);
};


class CFxRackWindow: public QWidget
{
Q_OBJECT

public:

  CVLevelMeter *level_meter;
  QTimer tm_level_meter;
  CFxRack *fx_rack;

  QCheckBox cb_l;
  QCheckBox cb_r;

  QPushButton *bt_apply; 

  CFxRackWindow();
  ~CFxRackWindow();

public slots:

  void tm_level_meter_timeout();
 // void apply_fx();
  
  void cb_l_changed (int value);
  void cb_r_changed (int value);

  void dial_gain_valueChanged (int value);


protected:

  void closeEvent (QCloseEvent *event);
  
signals:

  void apply_fx_button_pressed();
  
};


#endif
