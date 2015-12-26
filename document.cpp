/***************************************************************************
 *   2010 - 2015 by Peter Semiletov                                        *
 *   tea@list.ru                                                           *
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

#include <QSettings>
#include <QDebug>
#include <QPainter>
#include <QTime>
#include <QVBoxLayout>
#include <QUrl>
#include <QMimeData>
#include <QPushButton>
#include <QToolButton>
#include <QDial>

#include <math.h>
#include <samplerate.h>

#include "document.h"
#include "utils.h"
#include "gui_utils.h"

#include "logmemo.h"

#define FRAMES_PER_SECT_MAX 16

enum EMultiChannelMappingWAV //wav, flac, mp3
{
 SPEAKER_FRONT_LEFT = 0,
 SPEAKER_FRONT_RIGHT,
 SPEAKER_FRONT_CENTER,
 SPEAKER_LOW_FREQUENCY, //subwoofer
 SPEAKER_BACK_LEFT,
 SPEAKER_BACK_RIGHT,
 SPEAKER_FRONT_LEFT_OF_CENTER,
 SPEAKER_FRONT_RIGHT_OF_CENTER,
 SPEAKER_BACK_CENTER,
 SPEAKER_SIDE_LEFT,
 SPEAKER_SIDE_RIGHT,
 SPEAKER_LEFT_HEIGHT,
 SPEAKER_RIGHT_HEIGHT
};


QSettings *settings;
CDSP *dsp;
QHash <QString, QString> global_palette;
CDocumentHolder *documents;
CFxRackWindow *wnd_fxrack;

SRC_STATE *resampler;

int meter_cps;
int meter_msecs_delay;
int buffer_size_frames;
bool play_l;
bool play_r;
bool bypass_mixer;

//global variable with file formats
extern CFileFormats *file_formats;

//sound clipboard
CFloatBuffer *sound_clipboard; //incapsulate it?

//another global variable, also used at the transport code in eko.cpp
int transport_state = 0;



CUndoElement::CUndoElement()
{
  fb = 0;
  type = UNDO_UNDEFINED;
  frames_per_section = 0;

  cursor_frames = 0;
  start_frames = 0;
  end_frames = 0;
  
  selected = false;
}

 
CUndoElement::~CUndoElement()
{
  if (fb)
     delete fb;
}


CChannelSamples::CChannelSamples (size_t size)
{
  samples = new float [size];
  temp = 0.0f;
}


CChannelSamples::~CChannelSamples()
{
  delete [] samples;
} 


CChannelMinmax::CChannelMinmax (size_t size)
{
  max_values = new CChannelSamples (size);
  min_values = new CChannelSamples (size);
}


CChannelMinmax::~CChannelMinmax()
{
  delete max_values;
  delete min_values;
}


CMinmaxes::CMinmaxes (size_t size, size_t sections)
{
  values = new CChannelMinmax* [size];
  count = size;
  for (size_t i = 0; i < count; i++)
      values[i] = new CChannelMinmax (sections);  
}


CMinmaxes::~CMinmaxes()
{
  for (size_t i = 0; i < count; i++)
      delete values[i];

  delete [] values;     
} 


CTimeRuler::CTimeRuler (QWidget *parent): QWidget (parent)
{
  setMinimumHeight (1);
  setMaximumHeight (24);

  resize (width(), 24);
  init_state = true;
}


void CTimeRuler::paintEvent (QPaintEvent *event)
{
  if (init_state)
     {
      QWidget::paintEvent (event);  
      return; 
     }

if (! waveform && ! waveform->fb)
     {
      qDebug() << "! waveform";
      event->accept();
      return;
     }
  
  QImage img (width(), height(), QImage::Format_RGB32);

  QPainter painter (&img);

  painter.setFont (QFont ("Mono", 6));

  painter.setBrush (background_color);
  painter.drawRect (0, 0, width(), height());

  
  size_t sections = waveform->section_to - waveform->section_from;
  float sections_per_second = (float) waveform->fb->samplerate / waveform->frames_per_section;
 
 // qDebug() << "frames_per_section = " << waveform->frames_per_section;
  //qDebug() << "sections = " << sections;
  //qDebug() << "sections_per_second = " << sections_per_second;

  //how many mseconds fits to the current window sections range?
  
 // if (sections_per_second == 0) //nasty hack
   //  sections_per_second = 1;

  size_t seconds_start = waveform->section_from / sections_per_second;
  size_t seconds_end = waveform->section_to / sections_per_second;
    
  size_t seconds = seconds_end - seconds_start;

  bool measure_in_seconds = true;
  
//  if (seconds > 60)
  if (seconds > 15)
     measure_in_seconds = false;  
        
  painter.setPen (foreground_color);
        
  for (size_t x = 0; x < sections; x++)
      {
      if (measure_in_seconds)
         {
          if (x % (int)ceil (sections_per_second) == 0)
            {
             QPoint p1 (x, 1);
             QPoint p2 (x, 12);

             QTime t (0, 0);

             t = t.addSecs ((waveform->section_from + x) / sections_per_second);       

             painter.drawLine (p1, p2);
             painter.drawText (x + 2, 20, t.toString ("ss:zzz") + "s");
            }
         }
      else
         if (x % 60 == 0)
            {
             QPoint p1 (x, 1);
             QPoint p2 (x, 12);

             QTime t (0, 0);

             t = t.addSecs ((waveform->section_from + x) / sections_per_second);       

             painter.drawLine (p1, p2);
             painter.drawText (x + 2, 20, t.toString ("mm:ss") + "m");
            }
      }        

  QPainter painter2 (this);
  painter2.drawImage (0, 0, img);

  event->accept();
}


void CWaveform::paintEvent (QPaintEvent *event)
{
  if (init_state)
     {
      QWidget::paintEvent (event);
      return;
     } 

  QPainter painter (this);
  painter.drawImage (0, 0, waveform_image);

  int section = get_cursor_position_sections();

  painter.setPen (cl_cursor);
  
  int x = section - section_from;
  painter.drawLine (x, 1, x, height());

  if (selected)
  if (get_selection_start_sections() != get_selection_end_sections())
     {
      int s_start = get_selection_start_sections() - section_from;  //section start
      int s_end = get_selection_end_sections() - section_from;  //section end

      QPoint p1 (s_start, 0);
      QPoint p2 (s_end, height());
      QRect r (p1, p2);
      
      painter.fillRect (r, cl_waveform_selection_foreground);
     }
     
  event->accept();
}


void CWaveform::timer_timeout()
{
  if (play_looped) 
  if (fb->offset > sel_end_frames) 
     {
      /*qDebug() << "buffer_offset: " << fb->offset;
      qDebug() << "sel_end_samples: " << sel_end_frames;
      qDebug() << "diff: " << fb->offset - sel_end_frames;*/
     }
   
  if (get_cursor_position_sections() > section_to) 
     {
      scrollbar->setValue (scrollbar->value() + width());
     }
  else 
      update();
      
  set_cursorpos_text();
}  


CWaveform::CWaveform (QWidget *parent): QWidget (parent)
{
  play_looped = false;
  show_db = true;

  envelope_selected = -1;
  previous_mouse_pos_x = 0;

  selection_selected = 0;

  init_state = true;
  scale_factor = 1.0;
  section_from = 0;
  section_to = width() - 1;
  
  max_undos = 3;
    
  setMouseTracking (true);
  mouse_pressed = false;
  normal_cursor_shape = true;
  selected = false;

  minmaxes = 0;
  
  fb = new CFloatBuffer (32, settings->value ("def_channels", 1).toInt());
  fb->samplerate = settings->value ("def_samplerate", 44100).toInt();
  
  set_cursor_value (0);
  set_selstart_value (0);
  set_selend_value (0);

  connect(&timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  timer.setInterval (50); //сделать зависимым от текущего масштаба! was 40
}


CWaveform::~CWaveform()
{
  timer.stop();
  
  if (fb)
     delete fb;
  
  if (minmaxes)
     delete minmaxes;

  flush_undos();
}


void CWaveform::flush_undos()
{
  foreach (CUndoElement *el, undos)
           delete el;
}


inline bool x_nearby (int x, int pos, int width)
{
  int a = pos - width;
  int b = pos + width; 

  if (x >= a && x <= b)
     return true;
     
  return false;
}


void CWaveform::deselect()
{ 
  selected = false;

  sel_start_frames = fb->offset;
  sel_end_frames = fb->offset;
  
  selection_selected = 0;
}


void CWaveform::recalc_view()
{
  //qDebug() << "CWaveform::recalc_view() - start";

  if (! fb)
     return;
 
  /*
  size_t cursor_frames = cursor_position * frames_per_section;

  size_t sel_start_frames = selection_start * frames_per_section;
  size_t sel_end_frames = selection_end * frames_per_section;

  qDebug() << "frames_per_section = " << frames_per_section;
  qDebug() << "sel_start_frames = " << sel_start_frames;
  qDebug() << "sel_end_frames = " << sel_end_frames;
  qDebug() << "cursor_position = " << cursor_position;
  qDebug() << "cursor_frames = " << cursor_frames;
*/
  sections_total = width() * scale_factor;

  frames_per_section = ceil (fb->length_frames / sections_total);
  
  

  //qDebug() << "sel_start_frames = " << sel_start_frames;
  //qDebug() << "sel_end_frames = " << sel_end_frames;
 

  if (frames_per_section < FRAMES_PER_SECT_MAX)
     frames_per_section = FRAMES_PER_SECT_MAX;
     
      qDebug() << "frames_per_section == " << frames_per_section;

  //qDebug() << "cursor_frames = " << cursor_frames;

  scrollbar->setMinimum (0);
  scrollbar->setMaximum (sections_total - width());

  // << "CWaveform::recalc_view() - end";

}


void CWaveform::scale (int delta)
{
 //qDebug() << "CWaveform::scale - start";

  if (! fb)
     return;

 // if (frames_per_section == 0)
   //  return;

  int old_section_from = section_from;
  int old_frame_from = old_section_from * frames_per_section;

  if (delta > 0)
     scale_factor += 0.1f;
  else    
     scale_factor -= 0.1f;
    
  if (scale_factor < 1.0f)
     scale_factor = 1.0f;

  if ((width() * scale_factor) == fb->length_frames - 1) //can be scale factor so large?
      return;

  recalc_view();
  prepare_image();
  
  int new_section = old_frame_from / frames_per_section;   
  scrollbar->setValue (new_section);

  update();   
  timeruler->update();

  //qDebug() << "CWaveform::scale - end";
}


void CWaveform::wheelEvent (QWheelEvent *event)
{
  scale (event->delta());
  event->accept();
}  


void CWaveform::resizeEvent (QResizeEvent *event)
{
  recalc_view();
  
  section_from = scrollbar->value();
  section_to = width() + scrollbar->value();
  
  prepare_image();
}


void CWaveform::keyPressEvent (QKeyEvent *event)
{
  if (! fb)
     {
      event->accept();
      return;
     } 


  if (event->key() == Qt::Key_Delete)
     {
      wave_edit->waveform->delete_selected();
      event->accept();
      return;
     }


  if (event->key() == Qt::Key_Space)
     {
      wave_edit->doc->holder->transport_control->call_play_pause();

      event->accept();   
      return;
     }
  

  if (event->key() == Qt::Key_Return)
     {
      wave_edit->doc->holder->transport_control->call_stop();
      event->accept();   
      return;
     }


  if (event->key() == Qt::Key_Home)
     {
      set_cursor_value (0);

      scrollbar->setValue (0);         
             
      if (event->modifiers() & Qt::ShiftModifier)  
         {
          set_selstart_value (0);
          selected = true;   
         }
     
      update();

      set_cursorpos_text();
     
      event->accept();   
      return;
     }
 
  
  if (event->key() == Qt::Key_End)
     {
      set_cursor_value (sections_total - 1);
  
      scrollbar->setValue (sections_total - width());         
         
      if (event->modifiers() & Qt::ShiftModifier)  
         {
          set_selend_value (sections_total - 1);
          selected = true;   
         }
     
       update();

       set_cursorpos_text();
     
       event->accept();   
       return;
      }


  if (event->key() == Qt::Key_Left)
     {
      if (get_cursor_position_sections() != 0)
         set_cursor_by_section (get_cursor_position_sections() - 1);

      if (get_cursor_position_sections() == section_from)
         {
          if (scrollbar->value() != scrollbar->minimum())
             scrollbar->setValue (scrollbar->value() - 1);         
             
          event->accept();   
          return;
         }  

      if (event->modifiers() & Qt::AltModifier)  
         {
          if (get_selection_start_sections() != 0)
             {
              set_selstart_value (get_selection_start_sections() - 1);
              fix_selection_bounds();
             } 
              
          selected = true;
         }
         
       if (event->modifiers() & Qt::ShiftModifier)  
          {
          if (get_selection_start_sections() != 0)
              {
               set_selend_value (get_selection_end_sections() - 1);
               fix_selection_bounds();
              } 
              
           selected = true;   
          }
       
     
      update();

      set_cursorpos_text();
     
      event->accept();   
      return;
     }
 

  if (event->key() == Qt::Key_Right)
     {
      set_cursor_by_section (get_cursor_position_sections() + 1);

      if (get_cursor_position_sections() == sections_total)
         set_cursor_by_section (get_cursor_position_sections() - 1);
  
      if (get_cursor_position_sections() == section_to)
         {
          if (scrollbar->value() != scrollbar->maximum())
             scrollbar->setValue (scrollbar->value() + 1);         
             
          event->accept();
          return;
         }  
         
        
       if (event->modifiers() & Qt::AltModifier)  
         {
          if (get_selection_start_sections() != 0)
             {
              set_selstart_value (get_selection_start_sections() + 1);
              fix_selection_bounds();
             } 
              
          selected = true;
         }

         
       if (event->modifiers() & Qt::ShiftModifier)  
          {
           if (get_selection_end_sections() != sections_total)
              {
               set_selend_value (get_selection_end_sections() + 1);
               fix_selection_bounds();
              } 
              
           selected = true;   
          }
     
       update();

       set_cursorpos_text();
     
       event->accept();   
       return;
      }


  if (event->key() == Qt::Key_Plus)
     {
      scale (1);
      event->accept();   
      return;
     }
  

  if (event->key() == Qt::Key_Minus)
     {
      scale (-1);
      event->accept();   
      return;
     }
  

  if (event->key() == Qt::Key_Delete)
     {
      delete_selected();
      event->accept();
      set_cursorpos_text();
      return;
     }
 

  if (event->text() == "[")
      {
       set_selstart_value (get_cursor_position_sections());
       fix_selection_bounds();

       selected = true;
       update();
       
       event->accept();   
       return;
      }
   

  if (event->text() == "]")
      {
       set_selend_value (get_cursor_position_sections());

       fix_selection_bounds();

       selected = true;
       update();
             
       event->accept();   
       return;
      }
 
  QWidget::keyPressEvent (event);
}    


size_t CWaveform::frames_start()
{
  if (! selected)
     return 0;

  return sel_start_frames;
}


size_t CWaveform::frames_end()
{
  if (! fb)
     return 0;

  if (! selected)
     return fb->length_frames/* - 1*/;

  return sel_end_frames;
}


void CWaveform::copy_selected()
{
  if (! selected)
     return;

  if (sound_clipboard)
     delete sound_clipboard;
     
  sound_clipboard = fb->copy (frames_start(), frames_end() - frames_start());
}


void CWaveform::paste()
{
  if (! sound_clipboard)
     {
      qDebug() << "no sound data in clipboard"; 
      return;
     } 
  
  undo_take_shot (UNDO_PASTE);

  fb->paste_at (sound_clipboard, fb->offset);
  deselect();
     
  magic_update();
}


void CWaveform::cut_selected()
{
  if (! selected)
     return;

  copy_selected();
  
  undo_take_shot (UNDO_DELETE);
  
  delete_selected();
}


void CWaveform::magic_update()
{
  recalc_view();
  prepare_image();
  timeruler->update();
  update();
}


void CWaveform::load_color (const QString &fname)
{
  QHash <QString, QString> h = hash_load_keyval (fname);
  
  cl_waveform_background = QColor (h.value ("waveform_background", "white")); 
  cl_waveform_foreground = QColor (h.value ("waveform_foreground", "darkMagenta"));  
  cl_waveform_selection_foreground = QColor (h.value ("waveform_selection_foreground", "green"));  
  waveform_selection_alpha = h.value ("waveform_selection_alpha", "127").toInt();
  cl_waveform_selection_foreground.setAlpha (waveform_selection_alpha);
  cl_axis = QColor (h.value ("axis", "black"));  
  cl_cursor = QColor (h.value ("cursor", "red"));  
  cl_envelope = QColor (h.value ("envelope", "yellow"));  
  cl_text = QColor (h.value ("text", "black"));  
  cl_env_point_selected = QColor (h.value ("env_point_selected", "red"));  
  cl_shadow = QColor (h.value ("shadow_color", "grey"));  
  draw_shadow = h.value ("draw_shadow", "0").toInt();
  timeruler->background_color = QColor (h.value ("timeruler.background_color", cl_waveform_background.name())); 
  timeruler->foreground_color = QColor (h.value ("timeruler.foreground", cl_waveform_foreground.name())); 

  magic_update();
}
    

void CWaveform::fix_selection_bounds()
{
  if (selection_selected == 2 && (sel_start_frames > sel_end_frames))
     {
      size_t t = sel_start_frames;
      sel_start_frames = sel_end_frames;
      sel_end_frames = t;
      selection_selected = 1;
     }
  
  
  //size_t buffer_size_samples = buffer_size_frames * sound_buffer->channels;

 //выравнивание по размеру буфера, той или иной границы выделения
  
    //qDebug() << "sel_start_samples " << sel_start_samples;
    //qDebug() << "sel_end_samples " << sel_end_samples;
    //qDebug() << "total " << sound_buffer->samples_total;
  
    
  if (selection_selected == 2)  
     {
      if (fb->length_frames - sel_end_frames <= buffer_size_frames)
         {
          sel_end_frames = fb->length_frames;
              
          size_t i = sel_end_frames;
    
          while (i >= sel_start_frames)
                {
                 i -= buffer_size_frames;
                }
        
          sel_start_frames = i + buffer_size_frames;

            
           //  qDebug() << "fixed:";   

          //qDebug() << "sel_start_samples: " << sel_start_samples;
          //qDebug() << "sel_end_samples: " << sel_end_samples;
            
          return;
         }
      
      
      size_t i = sel_start_frames;

      while (i <= sel_end_frames)
            {
             i += buffer_size_frames;
            }
        
      sel_end_frames = i - buffer_size_frames;
     }
  else    
  if (selection_selected == 1)
     {
        
      if (sel_start_frames <= buffer_size_frames) //у начала
         {
          sel_start_frames = 0;
          
          size_t i = sel_start_frames;

          while (i <= sel_end_frames)
                {
                 i += buffer_size_frames;
                }
        
          sel_end_frames = i - buffer_size_frames;

         return;
        }      
      
      
      //иначе не у начала
  
      size_t i = sel_end_frames;
      
      while (i >= (sel_start_frames))
            {
             i -= buffer_size_frames;
            }
        
      sel_start_frames = i + buffer_size_frames;
   
      //qDebug() << sel_start_samples;
     }
}


void CWaveform::fix_selection_bounds_release()
{
  //round to
  
//  size_t buffer_size_samples = buffer_size_frames * sound_buffer->channels;
    
  if (selection_selected == 2)  
     {
      size_t i = sel_start_frames;
  
      while (i <= sel_end_frames)
            {
             i += buffer_size_frames;
            }
        
      sel_end_frames = i - buffer_size_frames;
     }
  else    
  if (selection_selected == 1)  
     {
      size_t i = sel_end_frames;
  
      while (i >= sel_start_frames)
            {
             i -= buffer_size_frames;
            }
        
      sel_start_frames = i + buffer_size_frames;
     }
}



void CWaveform::select_all()
{
  sel_start_frames = 0;
  sel_end_frames = fb->length_frames/* - 1*/;

  selected = true;
  update();
}


void CWaveform::undo_take_shot (int type, int param)
{
  qDebug() << "CWaveform::undo_take_shot - 1";

  CUndoElement *el = new CUndoElement;
  if (undos.count() == max_undos)
    {
     delete undos.at (undos.count() - 1);  
     undos.removeAt (undos.count() - 1);
    } 
        
  el->type = type;
  el->selected = selected;
  
  el->start_frames = frames_start();
  el->end_frames = frames_end();

  el->cursor_frames = fb->offset;
  el->frames_per_section = frames_per_section;

  /*
  if (type == UNDO_WHOLE)
     el->sound_buffer = sound_buffer->copy_to (0, sound_buffer->float_buffer->length_frames - 1);

  if (type == UNDO_DELETE)
     el->sound_buffer = sound_buffer->copy_to (frames_start(), frames_end());

  if (type == UNDO_MODIFY)
     el->sound_buffer = sound_buffer->copy_to (frames_start(), frames_end());

  if (type == UNDO_PASTE)
     {
      el->start_frames = sound_buffer->float_buffer->offset;
      el->end_frames = el->start_frames + sound_clipboard->float_buffer->length_frames - 1;
     }
*/


  if (type == UNDO_WHOLE)
     el->fb = fb->copy (0, fb->length_frames);

  if (type == UNDO_DELETE)
      el->fb = fb->copy (frames_start(), frames_end() - frames_start());

  if (type == UNDO_MODIFY)
     el->fb = fb->copy (frames_start(), frames_end() - frames_start());

//странно
  if (type == UNDO_PASTE)
     {
      el->start_frames = fb->offset;
      el->end_frames = el->start_frames + sound_clipboard->length_frames/* - 1*/;
     } 

//странно
  if (type == UNDO_INSERT)
     {
      el->start_frames = fb->offset;
      el->end_frames = param;
     }

  undos.prepend (el);
  
  qDebug() << "CWaveform::undo_take_shot - 2";

} 


void CWaveform::delete_selected()
{
  if (! selected)
     return;

  undo_take_shot (UNDO_DELETE);
  
  CFloatBuffer *tfb = fb->delete_range (frames_start(), frames_end());
  delete fb;
  fb = tfb;
    
  deselect();
  magic_update();
}


void CWaveform::redo()
{
  
}


void CWaveform::undo_top()
{

qDebug() << "void CWaveform::undo_top() - 1";

  if (undos.count() == 0)
     return;
    
  CUndoElement *el = undos.at (0);
 
  if (el->type == UNDO_UNDEFINED) 
     return;

  if (el->type == UNDO_WHOLE) 
     {
      delete fb;
      fb = el->fb->copy (0, el->fb->length_frames/* - 1*/);
     }  

  if (el->type == UNDO_DELETE) 
      fb->paste_at (el->fb, el->start_frames);

  if (el->type == UNDO_PASTE) 
     {
      CFloatBuffer *tfb = fb->delete_range (el->start_frames, el->end_frames);
      delete fb;
      fb = tfb;
      }
  
  if (el->type == UNDO_INSERT) 
     {
      CFloatBuffer *tfb = fb->delete_range (el->start_frames, el->end_frames);
      delete fb;
      fb = tfb;
      }
    
  if (el->type == UNDO_MODIFY) 
     fb->overwrite_at (el->fb, el->start_frames);
  
  recalc_view();

  selected = el->selected;
  
  sel_start_frames = el->start_frames;
  sel_end_frames = el->end_frames;
 
  if (fb)
     //set_cursor_value (el->sample_cursor * frames_per_section * sound_buffer->channels);
     fb->offset = el->cursor_frames;

  prepare_image();
  timeruler->update();
  update();

  undos.removeAt (0);
  delete el;
  
  qDebug() << "undos.count() " << undos.count(); 
  
  qDebug() << "void CWaveform::undo_top() - 2";

}


void CWaveform::set_cursor_to_frames (size_t frame)
{
  if (frame <= fb->length_frames)
      fb->offset = frame;
}


void CWaveform::set_statusbar_text()
{
  //qDebug() << "CWaveform::set_statusbar_text()";

  if (frames_per_section == 0)
     return;

  QString txt;

  int framesstart = frames_start();
  int framesend = frames_end();

  //qDebug() << "framesstart: " << framesstart; 
  //qDebug() << "framesend: " << framesstart; 

  float msecs_selstart = (float) framesstart / fb->samplerate * 1000;
  float msecs_selend = (float) framesend / fb->samplerate * 1000;
  float diff = msecs_selend - msecs_selstart;
  
  //qDebug() << "diff: " << diff;
  
  QTime a (0, 0);
  a = a.addMSecs ((int) msecs_selstart);       

  QTime b (0, 0);
  b = b.addMSecs ((int) msecs_selend);       

  QTime d (0, 0);
  d = d.addMSecs ((int) diff);       

  txt = a.toString ("hh:mm:ss.zzz");  
  txt.append (" <-> ");
  txt.append (b.toString ("hh:mm:ss.zzz"));     
  txt.append (" = ");
  txt.append (d.toString ("hh:mm:ss.zzz"));     
    
  wave_edit->doc->holder->l_status_bar->setText (txt);
  //documents->l_status_bar->setText (txt);
}

void CWaveform::set_cursorpos_text()
{
 // qDebug() << "CWaveform::set_cursorpos_text()";
  
  if (! fb)
     return;
     
  if (! fb->buffer[0])   
     return;
  
  size_t frames = fb->offset;
  
  float mseconds = (float) frames / fb->samplerate * 1000;
  
  QTime t (0, 0);
  t = t.addMSecs ((int)mseconds);       
  wave_edit->doc->holder->l_maintime->setText (t.toString ("hh:mm:ss.zzz"));
  
 // qDebug() << "buffer offset: " << sound_buffer->buffer_offset;
  //qDebug() << "total buffer size: " << sound_buffer->buffer_size;
  //qDebug() << "samples_total: " << sound_buffer->samples_total;
   
  //documents->l_maintime->setText (t.toString ("hh:mm:ss.zzz"));
}


//MOVE TO CWAVEFORM?
void CWaveEdit::dragEnterEvent (QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat ("text/uri-list"))
      event->acceptProposedAction();
}


CWaveEdit::CWaveEdit (QWidget *parent): QWidget (parent)
{
  QVBoxLayout *vlayout = new QVBoxLayout();

  waveform = new CWaveform (0);

  waveform->wave_edit = this;

  scb_horizontal = new QScrollBar (Qt::Horizontal);
  waveform->scrollbar = scb_horizontal;

  timeruler = new CTimeRuler (0);
  timeruler->waveform = waveform;
  waveform->timeruler = timeruler;

  vlayout->addWidget (timeruler);
  vlayout->addWidget (waveform);
  vlayout->addWidget (scb_horizontal);
  
  setLayout (vlayout);
  connect (scb_horizontal, SIGNAL(valueChanged(int)), this, SLOT(scb_horizontal_valueChanged(int)));
}


//PUT IT TO THE CWAVEFORM?
void CWaveEdit::dropEvent (QDropEvent *event)
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
             doc->holder->open_file (fName);
         }

  event->acceptProposedAction();
}
    

CWaveEdit::~CWaveEdit()
{
  delete waveform;
  delete timeruler;
}


bool CWaveEdit::isReadOnly()
{
  return false;
}


void CWaveEdit::scb_horizontal_valueChanged (int value)
{
  waveform->section_from = value;
  waveform->section_to = waveform->width() + value;
  
  waveform->init_state = false;
  timeruler->init_state = false;
  
  waveform->prepare_image();
  waveform->update();
  timeruler->update();
}  


void CDocument::update_title (bool fullname)
{
  QMainWindow *w = qobject_cast<QMainWindow *>(holder->parent_wnd);

  if (fullname)
     w->setWindowTitle (file_name);
  else
     w->setWindowTitle (QFileInfo (file_name).fileName());
  
  wave_edit->waveform->set_statusbar_text(); 
}


void CDocument::reload()
{
  if (file_exists (file_name))
      open_file (file_name);
}


bool CDocument::open_file (const QString &fileName, bool set_fname)
{
  qDebug() << "CDocument::open_file - start";

  CTio *tio = holder->tio_handler.get_for_fname (fileName);

  if (! tio)
     {
      holder->log->log (tr ("file type of %1 is not supported")
                           .arg (fileName));
      return false;
     }
  
  QTime tm;
  tm.start();
  
  CFloatBuffer *ff = tio->load (fileName);
  
  if (! ff)
     {
      holder->log->log (tr ("cannot open %1 because of: %2")
                           .arg (fileName)
                           .arg (tio->error_string));
      return false;
     }

  qDebug() << "ff->channels: " << ff->channels;
  qDebug() << "ff->length_frames: " << ff->length_frames;
  
  if (wave_edit->waveform->fb)
     delete wave_edit->waveform->fb;

  wave_edit->waveform->fb = ff;

//  wave_edit->waveform->sound_buffer->sndfile_format = tio->format;
  
//  wave_edit->waveform->sound_buffer->samplerate = tio->samplerate;
//  wave_edit->waveform->sound_buffer->default_channels_count = wave_edit->waveform->sound_buffer->float_buffer->channels;
    
  ronly = tio->ronly;  
    
  if (set_fname)
     {
      file_name = fileName;
      set_tab_caption (QFileInfo (file_name).fileName());
      holder->log->log (tr ("%1 is open").arg (file_name));
     }

  QMutableListIterator <QString> i (holder->recent_files);
  
  while (i.hasNext())
        {
         if (i.next() == file_name) 
            i.remove();
        }

///////////////////set up the scrollbar


  wave_edit->waveform->init_state = false;
  wave_edit->timeruler->init_state = false;

  wave_edit->waveform->magic_update();

  int elapsed = tm.elapsed();

  holder->log->log (tr ("elapsed: %1 ms").arg (elapsed));
  
  
  //qDebug() << "CDocument::open_file - end";

  return true;
}


bool CDocument::save_with_name (const QString &fileName)
{
 // qDebug() << "CDocument::save_with_name";

  if (ronly)
     {
      holder->log->log (tr ("cannot save %1 because of: %2")
                           .arg (fileName)
                           .arg ("read-only format"));
      return false;                     
      
     }

  int fmt = wave_edit->waveform->fb->sndfile_format & SF_FORMAT_TYPEMASK;  
  QString fname = fileName;
  QString ext = file_get_ext (fileName);
  QString fext = file_formats->hextensions.value (fmt);
  
  if (ext.isEmpty())
     fname = fname + "." + fext;
  else
      if (ext != fext)
          fname = change_file_ext (fname, fext);

  CTio *tio = holder->tio_handler.get_for_fname (fname);
  
  if (! tio)
     return false;

  tio->float_input_buffer = wave_edit->waveform->fb;
  //tio->samplerate = wave_edit->waveform->fb->samplerate;
  //tio->format = wave_edit->waveform->fb->sndfile_format;

  QTime tm;
  tm.start();
  
  if (! tio->save (fname))
     {
      holder->log->log (tr ("cannot save %1 because of: %2")
      //documents->log->log (tr ("cannot save %1 because of: %2")
      
                           .arg (fname)
                           .arg (tio->error_string));
      return false;
     }

  int elapsed = tm.elapsed();

  file_name = fname;

  set_tab_caption (QFileInfo (file_name).fileName());

  holder->log->log (tr ("%1 is saved").arg (file_name));
  holder->log->log (tr ("elapsed: %1 milliseconds").arg (elapsed));

  update_title();
  //update_status();

 // textEdit->document()->setModified (false);
 
  return true;
}


CDocument::CDocument (QObject *parent): QObject (parent)
{
  QString fname = tr ("new[%1]").arg (QTime::currentTime().toString ("hh-mm-ss"));
  
  ronly = false;
  file_name = fname;
  position = 0;
}


CDocument::~CDocument()
{
  /*if (textEdit->document()->isModified() && file_exists (file_name))
     {
      if (QMessageBox::warning (0, "EKO",
                                tr ("%1 has been modified.\n"
                                "Do you want to save your changes?")
                                .arg (file_name),
                                QMessageBox::Ok | QMessageBox::Default,
                                QMessageBox::Cancel | QMessageBox::Escape) == QMessageBox::Ok)
          save_with_name (file_name, charset);
     }
*/

 holder->add_to_recent (this);
 holder->update_recent_menu();
 
 delete wave_edit;

 int i = holder->tab_widget->indexOf (tab_page);
 if (i != -1)
     holder->tab_widget->removeTab (i);
}


void CDocument::create_new()
{
  wave_edit = new CWaveEdit;

  wave_edit->doc = this;
  wave_edit->waveform->load_color (holder->def_palette);
  
  int sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;

  int t = settings->value ("def_sndfile_format", sndfile_format).toInt();

  wave_edit->waveform->fb->sndfile_format = t;
  wave_edit->waveform->fb->samplerate = settings->value ("def_samplerate", 44100).toInt();

  int tab_index = holder->tab_widget->addTab (wave_edit, file_name);
  
  tab_page = holder->tab_widget->widget (tab_index);
  
  wave_edit->waveform->setFocus (Qt::OtherFocusReason);
  wave_edit->waveform->set_cursorpos_text();
}


int CDocument::get_tab_idx()
{
 return holder->tab_widget->indexOf (tab_page);
}


void CDocument::set_tab_caption (const QString &fileName)
{
  holder->tab_widget->setTabText (get_tab_idx(), fileName);
}


bool CDocument::save_with_name_plain (const QString &fileName)
{
  CTio *tio = holder->tio_handler.get_for_fname (fileName);

  tio->float_input_buffer = wave_edit->waveform->fb;
  //tio->samplerate = wave_edit->waveform->sound_buffer->samplerate;
  //tio->channels = wave_edit->waveform->sound_buffer->float_buffer->channels;
  //tio->format = wave_edit->waveform->sound_buffer->sndfile_format;
  
  if (! tio->save (fileName))
     {
      holder->log->log (tr ("cannot save %1 because of: %2")
                           .arg (fileName)
                           .arg (tio->error_string));
      return false;
     }

  return true;
}


void CDocumentHolder::reload_recent_list (void)
{
  if (! file_exists (recent_list_fname))
     return;

  recent_files = qstring_load (recent_list_fname).split ("\n");
}


CDocumentHolder::~CDocumentHolder()
{
  if (sound_clipboard)
     delete sound_clipboard;

  if (! list.isEmpty())
  while (! list.isEmpty())
        delete list.takeFirst();

  qstring_save (recent_list_fname, recent_files.join ("\n"));
}


CDocument* CDocumentHolder::create_new()
{
  CDocument *doc = new CDocument;

  doc->holder = this;
  list.append (doc);

  doc->create_new();

  current = doc;

  tab_widget->setCurrentIndex (tab_widget->indexOf (doc->tab_page));
  apply_settings_single (doc);

  doc->update_title();
  //doc->update_status();

  return doc;
}


CDocument* CDocumentHolder::open_file (const QString &fileName, bool set_fname)
{
  if (! file_exists (fileName))
     return NULL;

  if (! tio_handler.is_ext_supported (fileName))
     return NULL;

  foreach (CDocument *d, list)  
          if (d->file_name == fileName)
             {
              tab_widget->setCurrentIndex (tab_widget->indexOf (d->tab_page));
              return d;
             }


  CDocument *doc = create_new();

  doc->open_file (fileName, set_fname);
 
  doc->update_title();

  tab_widget->setCurrentIndex (tab_widget->indexOf (doc->tab_page));

  return doc;
}


void CDocumentHolder::close_current()
{
  int i = tab_widget->currentIndex();
  if (i < 0)
     return;

  CDocument *d = list[i];
  
  current = d;

  list.removeAt (i);
  delete d;
  
  current = 0;
}


CDocument* CDocumentHolder::get_current()
{
  int i = tab_widget->currentIndex();
  if (i < 0)
     {
      current = 0;
      return NULL;
     } 

  current = list[i];
  return current;
}


void CDocumentHolder::apply_settings_single (CDocument *d)
{
  d->wave_edit->waveform->show_db = settings->value ("meterbar_show_db", true).toBool();
  d->wave_edit->waveform->max_undos = settings->value ("max_undos", 6).toInt();
  d->wave_edit->waveform->prepare_image();
  d->wave_edit->waveform->update();
}


void CDocumentHolder::apply_settings()
{
  foreach (CDocument *d, list)
          apply_settings_single (d);
}


void CDocumentHolder::add_to_recent (CDocument *d)
{
  if (! file_exists (d->file_name))
     return;

  recent_files.prepend (d->file_name);
  
  if (recent_files.size() > 13)
     recent_files.removeLast();
}


void CDocumentHolder::update_recent_menu()
{
  recent_menu->clear();
  create_menu_from_list (this, recent_menu, recent_files, SLOT(open_recent()));
}


void CDocumentHolder::open_recent()
{
  QAction *Act = qobject_cast<QAction *>(sender());

  int i = recent_files.indexOf (Act->text ());
  if (i == -1)
     return;

  open_file (recent_files[i]);
  update_recent_menu();
}


void CDocumentHolder::save_to_session (const QString &fileName)
{
  if (list.size() < 0)
     return;

  fname_current_session = fileName;
  QString l;

  foreach (CDocument *d, list)
          {
           l += d->file_name;
           l += "\n";
          }
  
  qstring_save (fileName, l.trimmed());
}


void CDocumentHolder::load_from_session (const QString &fileName)
{
  if (! file_exists (fileName))
     return;

  QStringList l = qstring_load (fileName).split("\n");
  int c = l.size();
  if (c < 0)
     return;

  foreach (QString t, l)  
          open_file (t);

  fname_current_session = fileName;
}


void CDocumentHolder::load_palette (const QString &fileName)
{
  def_palette = fileName;
  
  foreach (CDocument *d, list)
          d->wave_edit->waveform->load_color (fileName);
}


CDocumentHolder::CDocumentHolder (QObject *parent):  QObject (parent)
{
  sound_clipboard = new CFloatBuffer (1, settings->value ("def_channels", 1).toInt()); 
  current = 0;
}
 

void CTransportControl::call_play_pause()
{
  emit play_pause();
}


void CTransportControl::call_stop()
{
  emit stop();
}


void CWaveform::set_cursor_value (size_t section)
{
  if (fb)
     fb->offset = section * frames_per_section;
}


void CWaveform::set_selstart_value (size_t section)
{
  if (fb)
     sel_start_frames = section * frames_per_section;
}


void CWaveform::set_selend_value (size_t section)
{
  if (fb)
     sel_end_frames = section * frames_per_section;
}



void CFxRackWindow::tm_level_meter_timeout()
{
  if (! level_meter)
      return;   

  level_meter->update();
}

//FIXME: make meter bar non-linear
void CVLevelMeter::update_scale_image()
{
  QImage im (scale_width, height(), QImage::Format_RGB32);

  QPainter painter (&im);

  painter.fillRect (0, 0, scale_width, height(), Qt::white);

  painter.setPen (Qt::black);
  painter.setFont (QFont ("Mono", 6));
 
  int ten = get_value (height(), 5);  
                    
  int percentage = 105;
  float ff = 0.0f;
  
  for (int y = 0; y < height(); y++)
      {
       if (! (y % ten))
          {
           percentage -= 5;
          
           ff = (1.0 / 100) * percentage;
                                            
           QPoint p1 (1, y);
           QPoint p2 (scale_width, y);
           painter.drawLine (p1, p2);
               
           painter.drawText (QPoint (1, y), QString::number (float2db (ff), 'g', 2)); //dB
          }
       }         
     
  img_bar = im;
}


void CVLevelMeter::resizeEvent(QResizeEvent *event)
{
  update_scale_image();
}


CVLevelMeter::CVLevelMeter (QWidget *parent)
{
  peak_l = 0;
  peak_r = 0;

  scale_width = 42;
  bars_width = 48;

  setMinimumWidth (scale_width + bars_width);

  resize (scale_width + bars_width, 256);
}


#define FALLOFF_COEF 0.05f

void CVLevelMeter::paintEvent (QPaintEvent *event)
{
  if (pl > peak_l)
     peak_l = pl;
  else   
      peak_l -= FALLOFF_COEF;
     
  if (pr > peak_r)
     peak_r = pr;
  else   
      peak_r -= FALLOFF_COEF;

  if (peak_l < -1 || peak_l > 1)
     peak_l = 0;

  if (peak_r < -1 || peak_r > 1)
     peak_r = 0;

  QPainter painter (this);   
   
  int h = height();
  
  painter.fillRect (scale_width, 0, width(), height(), Qt::white);
       
  int lenl = h - (int)(peak_l * 1.0f * h);
  int lenr = h - (int)(peak_r * 1.0f * h);
      
  QPoint ltop (scale_width, height());
  QPoint lbottom (scale_width + (width() - scale_width)  / 2, lenl);
  
  QPoint rtop (scale_width + (width() - scale_width) / 2, height());
  QPoint rbottom (width(), lenr);
  
  QRect l (ltop, lbottom);
  QRect r (rtop, rbottom);
    
  painter.fillRect (l, Qt::green);
  painter.fillRect (r, Qt::darkGreen);

  painter.drawImage (1, 1, img_bar);
 
  event->accept();  
}


CFxRackWindow::~CFxRackWindow()
{
  delete fx_rack;
}



void CFxRackWindow::cb_l_changed (int value)
{
  play_l = value; 
}


void CFxRackWindow::cb_r_changed (int value)
{
  play_r = value; 
}


void CFxRackWindow::dial_gain_valueChanged (int value)
{
  float f = 1.0f;
  if (value == 0)
     {
      dsp->gain = 1.0f;
      return;
     }

   f = db2lin (value);

   dsp->gain = f;
}

/*
void CFxRackWindow::dial_pan_valueChanged (int value)
{
   //0 - left, 0.5 - middle, 1 - right
   
   if (value == 0)
      {
       dsp->pan = 0.5;
       return;
      }
   
   if (value < 0)
      {
       int v = (100 - value * -1);
       dsp->pan = get_fvalue (0.5f, v);
      }
   
   if (value > 0)
      {
       dsp->pan = get_fvalue (0.5f, value) + 0.5f;
      }
}
*/

CFxRackWindow::CFxRackWindow()
{
  QHBoxLayout *h_mainbox = new QHBoxLayout;
  setLayout (h_mainbox);

  QVBoxLayout *v_box = new QVBoxLayout;
  
  h_mainbox->addLayout (v_box);
  
  level_meter = new CVLevelMeter (this);
  
  QPushButton *bt_apply = new QPushButton (tr ("Apply"));
  connect (bt_apply, SIGNAL(clicked()), this, SLOT(apply_fx()));

  fx_rack = new CFxRack;

  v_box->addWidget (fx_rack->inserts);
  v_box->addWidget (bt_apply);

  QVBoxLayout *v_meter = new QVBoxLayout;
  v_meter->addWidget (level_meter); 
  
  cb_l.setText ("L");
  cb_r.setText ("R");
  cb_l.setChecked (true);
  cb_r.setChecked (true);
  
  connect(&cb_l, SIGNAL(stateChanged (int )), this, SLOT(cb_l_changed (int )));
  connect(&cb_r, SIGNAL(stateChanged (int )), this, SLOT(cb_r_changed (int )));

  
  QHBoxLayout *h_lr = new QHBoxLayout;
  h_lr->addWidget (&cb_l);
  h_lr->addWidget (&cb_r);
  
  v_meter->addLayout (h_lr); 

  h_mainbox->addLayout (v_meter); 
  
  connect(&tm_level_meter, SIGNAL(timeout()), this, SLOT(tm_level_meter_timeout()));
  
  //tm_level_meter.setInterval (meter_msecs_delay);
  tm_level_meter.setInterval (60);


  QVBoxLayout *v_vol = new QVBoxLayout;
  QLabel *l_vol = new QLabel (tr ("Volume"));
  QDial *dial_volume = new QDial;
  dial_volume->setWrapping (false);
  connect (dial_volume, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_volume->setRange (-26, 26);

  
  v_vol->addWidget (l_vol);
  v_vol->addWidget (dial_volume);
  
/*
  QVBoxLayout *v_pan = new QVBoxLayout;
  QLabel *l_pan = new QLabel (tr ("Pan"));
  QDial *dial_pan = new QDial;
  dial_pan->setWrapping (false);
  connect (dial_pan, SIGNAL(valueChanged(int)), this, SLOT(dial_pan_valueChanged(int)));
  dial_pan->setRange (-100, 100);

  
  v_pan->addWidget (l_pan);
  v_pan->addWidget (dial_pan);
  */
  QHBoxLayout *h_volpan = new QHBoxLayout;
  h_volpan->addLayout (v_vol);
  //h_volpan->addLayout (v_pan);
    
  v_box->addLayout (h_volpan);

  setWindowTitle (tr ("Mixer"));
}


void CFxRackWindow::closeEvent (QCloseEvent *event)
{
  event->accept();
}



void CFxRackWindow::apply_fx()
{
  CDocument *d = documents->get_current();
  if (! d)
     return;

  dsp->process_whole_document (d);
  wnd_fxrack->fx_rack->bypass_all();
}


//ГЛЮЧИТ!!!
//process the whole file or the selection
bool CDSP::process_whole_document (CDocument *d)
{
  if (! d)
     return false;

//here we work with short buffer to process it and output to it
  
  d->wave_edit->waveform->undo_take_shot (UNDO_MODIFY);

  //size_t portion_size = buffer_size_frames;

  //qDebug() << "portion_size =" << portion_size;

  size_t frames_start = d->wave_edit->waveform->frames_start();
  size_t frames_end = d->wave_edit->waveform->frames_end();
  
  //qDebug() << "frames_start: " << frames_start;
  //qDebug() << "frames_end: " << frames_end;

  //size_t nframes = d->wave_edit->waveform->fb->length_frames;
  
  d->wave_edit->waveform->fb->pbuffer_reset();
    

//FIXIT!!! CHECK IF sample_end > sample_start

  if (d->wave_edit->waveform->selected)
     {
      //nframes = frames_end - frames_start;
      //d->wave_edit->waveform->fb->offset = frames_start;
     }

  
  //d->wave_edit->waveform->fb->pbuffer_inc (d->wave_edit->waveform->fb->offset);
  

  //qDebug() << "offset = " << offset;
  //qDebug() << "nsamples = " << nsamples;

  ////////////call fx chain

  //d->wave_edit->waveform->fb->offset = frames_start;

  d->wave_edit->waveform->fb->pbuffer_inc (frames_start); 

  while (d->wave_edit->waveform->fb->offset < frames_end)
        {
         //qDebug() << "offset = " << d->wave_edit->waveform->fb->offset;

         //size_t diff = nframes - d->wave_edit->waveform->fb->offset;
         //if (diff < portion_size)
           // portion_size = diff;

         for (int i = 0; i < wnd_fxrack->fx_rack->effects.count(); i++)
             {
              if (! wnd_fxrack->fx_rack->effects[i]->bypass)
                 {
                  wnd_fxrack->fx_rack->effects[i]->realtime = false;
                 
                  wnd_fxrack->fx_rack->effects[i]->channels = d->wave_edit->waveform->fb->channels;
                  wnd_fxrack->fx_rack->effects[i]->samplerate = d->wave_edit->waveform->fb->samplerate;
                  
                  //wnd_fxrack->fx_rack->effects[i]->execute (d->wave_edit->waveform->fb->pbuffer,
                    //                                        d->wave_edit->waveform->fb->pbuffer, 
                      //                                      buffer_size_frames);
                  wnd_fxrack->fx_rack->effects[i]->execute (d->wave_edit->waveform->fb->pbuffer,
                                                            d->wave_edit->waveform->fb->pbuffer, 
                                                            buffer_size_frames);
                     
                 }
             }

         d->wave_edit->waveform->fb->pbuffer_inc (buffer_size_frames);  
        }


  for (int i = 0; i < wnd_fxrack->fx_rack->effects.count(); i++)
      {
       if (! wnd_fxrack->fx_rack->effects[i]->bypass)
           wnd_fxrack->fx_rack->effects[i]->bypass = true;
      }


  d->wave_edit->waveform->fb->offset = 0;
  d->wave_edit->waveform->magic_update();

  return true;
}


const QModelIndex CFxRack::index_from_name (const QString &name)
{
  QList <QStandardItem *> lst = model->findItems (name);
  if (lst.size() > 0)
     return model->indexFromItem (lst[0]);
  else
      return QModelIndex();
}


void CFxRack::tv_activated (const QModelIndex &index)
{
  emit fx_activated (index.data().toString());
  //qDebug() << index.data().toString();

  int i = index.row();
  if (i != -1)
     effects.at (i)->show_ui();
}


void CFxRack::add_entry (AFx *f, bool checked)
{
  QStandardItem *item = new QStandardItem (f->name);
  item->setCheckable (true);

  if (checked)
     item->setCheckState (Qt::Checked);

  item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable |
                  Qt::ItemIsDropEnabled);

  model->appendRow (item);
}


void CFxRack::ins_entry (AFx *f)
{
  QStandardItem *item = new QStandardItem (f->name);
  item->setCheckable (true);
  item->setCheckState (Qt::Checked);

  item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable |
                  Qt::ItemIsDropEnabled);


  int i = get_sel_index();
  if (i == -1)
     {
      model->appendRow (item);
//      effects.append (f->self_create (d->wave_edit->waveform->sound_buffer->samplerate, d->wave_edit->waveform->sound_buffer->channels));
      effects.append (f->self_create (44100)); //def vals

     }
  else
      {
       model->insertRow (i, item);
//       effects.insert (i, f->self_create (d->wave_edit->waveform->sound_buffer->samplerate, d->wave_edit->waveform->sound_buffer->channels));
       effects.insert (i, f->self_create (44100));

      }
}

//FIXME!!!
void CFxRack::bt_up_clicked()
{
/*  CDocument *d = documents->get_current();
  if (! d)
     return;
*/
  if (! tree_view->selectionModel()->hasSelection())
     return;

  QModelIndex index = tree_view->selectionModel()->currentIndex();
  int row = index.row();
  if (row == 0)
     return;

  QList<QStandardItem *> l = model->takeRow (row);

  //QStandardItem *item = model->takeItem (row);

  int new_row = row - 1;

  model->insertRow (new_row, l[0]);

  effects.swap (row, new_row);

 // qDebug() << "FX: end";
}


//FIXME!!!
void CFxRack::bt_down_clicked()
{

  if (! tree_view->selectionModel()->hasSelection())
     return;

  QModelIndex index = tree_view->selectionModel()->currentIndex();
  int row = index.row();
  if (row == model->rowCount() - 1)
     return;

  QList<QStandardItem *> l = model->takeRow (row);

//  qDebug() << "before: " << model->rowCount();

  int new_row = row + 1 ;

  model->insertRow (new_row, l[0]);

//  qDebug() << "after: " << model->rowCount();

  effects.swap (row, new_row);
}


CFxRack::CFxRack (QObject *parent): QObject (parent)
{
  avail_fx = new CFxList;

  model = new QStandardItemModel (0, 1, parent);

  connect (model, SIGNAL(dataChanged (const QModelIndex &, const QModelIndex &)),
          this, SLOT(bypass_dataChanged (const QModelIndex &, const QModelIndex &)));


  tree_view = new CFxTreeView;
  tree_view->setHeaderHidden (true);
  //tree_view->rack = this;
  tree_view->setRootIsDecorated (false);
  tree_view->setModel (model);

  tree_view->setDragEnabled (true);

  //tree_view->header()->setResizeMode (QHeaderView::ResizeToContents);
  //tree_view->header()->setStretchLastSection (false);

  tree_view->setSelectionMode (QAbstractItemView::ExtendedSelection);
  tree_view->setSelectionBehavior (QAbstractItemView::SelectRows);

  connect (tree_view, SIGNAL(activated(const QModelIndex &)), this, SLOT(tv_activated(const QModelIndex &)));


  inserts = new QWidget;

  QVBoxLayout *v_box = new QVBoxLayout;
  inserts->setLayout (v_box);

  QPushButton *bt_add = new QPushButton ("+");
  QPushButton *bt_del = new QPushButton ("-");

  QToolButton *bt_up = new QToolButton;
  QToolButton *bt_down = new QToolButton;
  bt_up->setArrowType (Qt::UpArrow);
  bt_down->setArrowType (Qt::DownArrow);

  connect (bt_up, SIGNAL(clicked()), this, SLOT(bt_up_clicked()));
  connect (bt_down, SIGNAL(clicked()), this, SLOT(bt_down_clicked()));

  connect (bt_add, SIGNAL(clicked()), this, SLOT(add_fx()));
  connect (bt_del, SIGNAL(clicked()), this, SLOT(del_fx()));


  QHBoxLayout *h_box = new QHBoxLayout;
  h_box->addWidget (bt_add);
  h_box->addWidget (bt_del);
  h_box->addWidget (bt_up);
  h_box->addWidget (bt_down);

  v_box->addWidget (tree_view);
  v_box->addLayout (h_box);
}


void CFxRack::add_fx()
{
  CTextListWindow w (tr ("Select"), tr ("Available effects"));

  w.list->addItems (avail_fx->names());

  int result = w.exec();

  if (result != QDialog::Accepted)
      return;

  AFx *f = avail_fx->find_by_name (w.list->currentItem()->text());

  ins_entry (f);

  print_fx_list();
}


void CFxRack::del_fx()
{
  int i = get_sel_index();
  if (i == -1)
     return;

  QList<QStandardItem *> l = model->takeRow (i);
  delete l[0];

  AFx *f = effects.takeAt (i);
  if (f)
     delete f;

  print_fx_list();
}


CFxRack::~CFxRack()
{
  foreach (AFx *f, effects)
         {
          f->wnd_ui->close();
          delete f;
         }
 
  delete avail_fx;
}


QString CFxRack::get_sel_fname()
{
  if (! tree_view->selectionModel()->hasSelection())
      return QString();

  QModelIndex index = tree_view->selectionModel()->currentIndex();
  return index.data().toString();
}


const QModelIndex CFxRack::index_from_idx (int idx)
{
  QStandardItem *item = model->item (idx);
  if (item)
     return model->indexFromItem (item);
  else
      return QModelIndex();
}


int CFxRack::get_sel_index()
{
  if (! tree_view->selectionModel()->hasSelection())
     return -1;

  QModelIndex index = tree_view->selectionModel()->currentIndex();
  return index.row();
}


void CFxTreeView::mouseMoveEvent (QMouseEvent *event)
{
 /* if (! (event->buttons() & Qt::LeftButton))
     return;

  QStringList l = fman->get_sel_fnames();
  if (l.size() < 1)
     return;

  QDrag *drag = new QDrag (this);
  QMimeData *mimeData = new QMimeData;

  QList <QUrl> list;

  foreach (QString fn, l)
           list.append (QUrl::fromLocalFile (fn));

  mimeData->setUrls (list);
  drag->setMimeData (mimeData);

  if (drag->exec (Qt::CopyAction |
                  Qt::MoveAction |
                  Qt::LinkAction) == Qt::MoveAction)
     fman->refresh();
    */
  event->accept();
}


void CFxRack::print_fx_list()
{
  foreach (AFx *f, effects)
          qDebug() << f->name;
}


void CFxRack::bypass_dataChanged (const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
  bool b = model->data (topLeft, Qt::CheckStateRole).toInt();

  //qDebug() << model->data (topLeft, Qt::DisplayRole).toString() << " = " << b;
  //qDebug() << "row = " << topLeft.row();

  effects[topLeft.row()]->bypass = ! b;

  //if (effects[topLeft.row()]->bypass)
    // qDebug() << "bypassed";
}


void CFxRack::bypass_all (bool mode)
{
  for (int row = 0; row < model->rowCount(); row++)
      {
       QStandardItem *si = model->item (row);
       if (mode)
          si->setCheckState (Qt::Unchecked);
       else
          si->setCheckState (Qt::Checked);
      }
}


void CFxRack::set_state_all (FxState state)
{
  foreach (AFx *f, effects)
          {
           f->set_state (state);
          }
}


size_t CDSP::process (size_t nframes)
{
 // qDebug() << "CDSP::process -- start";
  
  if (nframes == 0)
     return 0;

  if (transport_state == STATE_EXIT)
     return 0;

  if (! documents->current)
     return 0;
   
  maxl = 0.0f;
  maxr = 0.0f;

//here we work with short buffer to process it and output to it

  size_t frames = nframes;
//  qDebug() << "nsamples: " << nsamples;
  
  size_t tail = documents->current->wave_edit->waveform->fb->length_frames
                - documents->current->wave_edit->waveform->fb->offset;
                
  tail = tail - frames; 
  
  if (tail < nframes)
     frames = tail;
   
  //just copy
  
  documents->current->wave_edit->waveform->fb->copy_to_pos (
                     temp_float_buffer,
                     documents->current->wave_edit->waveform->fb->offset, frames, 0);
  
  
  //memcpy (temp_buffer, psource, nsamples * sizeof (float));

    
////////////call fx chain

  //temp_float_buffer->pbuffer_reset();

  if (! bypass_mixer && documents->current)
  for (int i = 0; i < wnd_fxrack->fx_rack->effects.count(); i++)
      {
       if (! wnd_fxrack->fx_rack->effects[i]->bypass)
          { 
           wnd_fxrack->fx_rack->effects[i]->realtime = true;
           wnd_fxrack->fx_rack->effects[i]->channels = documents->current->wave_edit->waveform->fb->channels;
           wnd_fxrack->fx_rack->effects[i]->samplerate = documents->current->wave_edit->waveform->fb->samplerate;
           wnd_fxrack->fx_rack->effects[i]->execute (temp_float_buffer->buffer, temp_float_buffer->buffer, frames);
          }
      }

 
  for (size_t i = 0; i < frames; i++)
      {
       if (float_greater_than (temp_float_buffer->buffer[0][i], maxl))
           maxl = temp_float_buffer->buffer[0][i];
      
      if (temp_float_buffer->channels == 2)
          if (float_greater_than (temp_float_buffer->buffer[1][i], maxr))
             maxr = temp_float_buffer->buffer[1][i];
              
      temp_float_buffer->buffer[0][i] *= gain;  
      
      if (temp_float_buffer->channels == 2)
          temp_float_buffer->buffer[1][i] *= gain;  
     }

  if (wnd_fxrack->level_meter)         
     {
      wnd_fxrack->level_meter->pl = maxl;
      wnd_fxrack->level_meter->pr = maxr;
     }
                

  documents->current->wave_edit->waveform->fb->offset += frames;
 
  //qDebug() << "CDSP::process -- end";
 
 
  return frames;
}


size_t CDSP::process_rec (float *buffer, size_t channels, size_t nframes)
{
  //qDebug() << "CDSP::process -- start";
  /*
  if (nframes == 0)
     return 0;

  if (transport_state == STATE_EXIT)
     return 0;

  
  int *pchannels = (int *)buffer;
  float *pout_l = (float *)pchannels[0];
  float *pout_r = (float *)pchannels[1];
   
  maxl = 0.0f;
  maxr = 0.0f;

//here we work with short buffer to process it and output to it

  size_t nsamples = nframes * channels;
                
  for (size_t i = 0; i < nsamples; i++)
      {
       if (float_greater_than (pout_l[i], maxl))
           maxl = pout_l[i];
           
       if (float_greater_than (pout_r[i], maxr))
           maxr = pout_r[i];
      }
           
  if (wnd_fxrack->level_meter)         
     {
      wnd_fxrack->level_meter->pl = maxl;
      wnd_fxrack->level_meter->pr = maxr;
     }
               

  return nframes;*/
 
  if (nframes == 0)
     return 0;

  if (transport_state == STATE_EXIT)
     return 0;

   
  maxl = 0.0f;
  maxr = 0.0f;

//here we work with short buffer to process it and output to it

  size_t nsamples = nframes * channels;
                
  if (channels == 1)
     for (size_t i = 0; i < nsamples; i++)
         {
          if (float_greater_than (buffer[i], maxl))
              maxl = buffer[i];
         }
           
           
  size_t i = 0;

  if (channels == 2)
     do
       {
        if (float_less_than (maxl, buffer[i]))
            maxl = buffer[i];
        i++;
        if (float_less_than (maxr, buffer[i]))
            maxr = buffer[i];
        i++;
       }
    while (i < nsamples);


  if (wnd_fxrack->level_meter)         
     {
      wnd_fxrack->level_meter->pl = maxl;
      wnd_fxrack->level_meter->pr = maxr;
     }

  return nframes;
}



void CWaveform::prepare_image()
{
//  qDebug() << "CWaveform::prepare_image() - start";

  if (! fb)
    {
      qDebug() << "! sound_buffer";
      return;
     } 


  if (frames_per_section == 0)
     return;

 // qDebug() << "sb buffer size: " << sound_buffer->buffer_size;

  size_t sections = section_to - section_from;
  int image_height = height();
  int channel_height = image_height / fb->channels;
  
  if (minmaxes)
     delete minmaxes;

  minmaxes = new CMinmaxes (fb->channels, sections);
  
  //QTime tm;
  //tm.start();
  
  size_t frame = 0;
  size_t section = 0;
  size_t frameno = section_from * frames_per_section/* - 1*/;
 
 
 //qDebug() << "sections: " << sections;
 
 //переписать это в for обратив в проход по всему буферу
  while (section + 1 < sections)
        {
         frameno++;
          
         for (size_t ch = 0; ch < fb->channels; ch++)    
             {
              //заменить на fmin fmax
              if (fb->buffer[ch][frameno] < minmaxes->values[ch]->min_values->temp)
                 minmaxes->values[ch]->min_values->temp = fb->buffer[ch][frameno];
              else   
              if (fb->buffer[ch][frameno] > minmaxes->values[ch]->max_values->temp)
                 minmaxes->values[ch]->max_values->temp = fb->buffer[ch][frameno];
             }
           

         if (frame == frames_per_section)
            {
             section++;
             frame = 0;
             for (size_t ch = 0; ch < fb->channels; ch++)    
                 { 
                  minmaxes->values[ch]->min_values->samples[section] = minmaxes->values[ch]->min_values->temp;
                  minmaxes->values[ch]->max_values->samples[section] = minmaxes->values[ch]->max_values->temp;
               
                  minmaxes->values[ch]->min_values->temp = 0;
                  minmaxes->values[ch]->max_values->temp = 0;
                 }

             continue;
            }

          frame++;
         }

 // << "3";

   //int msecs = tm.elapsed();
    //qDebug() << "time: " << msecs;
   


/*
  QTime tm;
  tm.start();

  //size_t frame = 0;
  size_t section = -1;
  size_t sampleno = section_from * (frames_per_section * sound_buffer->channels) - 1;

  size_t frame_start = section_from * frames_per_section;
  size_t frame_end = section_to * frames_per_section - 1;

#pragma omp for schedule(dynamic)
  for (size_t i = frame_start; i < frame_end; i++)
      {
       //qDebug() << i;
       for (int ch = 0; ch < sound_buffer->channels; ch++)    
             {
              sampleno++;

              if (sound_buffer->buffer[sampleno] < minmaxes->values[ch]->min_values->temp)
                 minmaxes->values[ch]->min_values->temp = sound_buffer->buffer[sampleno];
              else   
              if (sound_buffer->buffer[sampleno] > minmaxes->values[ch]->max_values->temp)
                 minmaxes->values[ch]->max_values->temp = sound_buffer->buffer[sampleno];
             }
             
             
         if (i % frames_per_section == 0)
            {
             section++;
             
           //  qDebug() << section;
             
             for (int ch = 0; ch < sound_buffer->channels; ch++)    
                 { 
                  minmaxes->values[ch]->min_values->samples[section] = minmaxes->values[ch]->min_values->temp;
                  minmaxes->values[ch]->max_values->samples[section] = minmaxes->values[ch]->max_values->temp;
               
                  minmaxes->values[ch]->min_values->temp = 0;
                  minmaxes->values[ch]->max_values->temp = 0;
                 }
            }
      }
      
   int msecs = tm.elapsed();
   qDebug() << "time: " << msecs;
      
*/

 

  QImage img (width(), image_height, QImage::Format_RGB32);
  QPainter painter (&img);
  
  painter.setPen (cl_waveform_foreground);
    
  img.fill (cl_waveform_background.rgb()); 
    
  for (int ch = 0; ch < fb->channels; ch++)    
  for (size_t x = 0; x < sections; x++)
      {
       int ymax = channel_height - (int)((minmaxes->values[ch]->max_values->samples[x] + 1) * 0.5f * channel_height);
       int ymin = channel_height - (int)((minmaxes->values[ch]->min_values->samples[x] + 1) * 0.5f * channel_height);

  	   QPoint p1 (x, ymin + channel_height * ch);
       QPoint p2 (x, ymax + channel_height * ch);
       
  		   /*
  		   if ((minmaxes->values[ch]->max_values->samples[x] >= 1) || (minmaxes->values[ch]->min_values->samples[x] <= -1))
       painter.setPen (QColor (Qt::red));
       else
       painter.setPen (cl_waveform_foreground);
*/

       painter.drawLine (p1, p2);
      }        


//draw the amplitude meter bar  
//FIXME!!!
  painter.setPen (cl_text);
  //painter.setFont (QFont ("Mono", 5));

  QFont tfont ("Mono");
  tfont.setPixelSize (8);

  painter.setFont (tfont);

   
  //int ten = get_value (channel_height / 2, 10) / 2;  
       
  for (int ch = 0; ch < fb->channels; ch++)    
      {
      
      //draw axis
       int yaxe = image_height / (ch + 1) - channel_height / 2;
       
       QPoint p1 (0, yaxe);
       QPoint p2 (width(), yaxe);
       painter.drawLine (p1, p2);
                        
       int y_channel_origin = channel_height * (ch + 1) - channel_height;
    
       int db = -27;
    
       while (db <= 0)
           {
            float linval = db2lin (db);
            
            int ymin = channel_height - (int)((linval + 1) * .5 * channel_height) + y_channel_origin;
            int y = ymin;  

            QPoint p1 (1, y);
            QPoint p2 (5, y);
            painter.drawLine (p1, p2);
       
            if (draw_shadow)
               {    
                painter.setPen (cl_shadow);
                painter.drawText (QPoint (2, 1 + y), QString::number (db, 'g', 2)); //dB
                painter.setPen (cl_text);
               }                  
                    
            painter.drawText (QPoint (1, y), QString::number (db, 'g', 2)); //dB
         
            if (db >= -9)
               db++;
            else
                db += 5;
    
           }            
       }  
  
  //draw vol env
  
  int old_point_x = -1;
  int old_point_y = -1;

  if (env_vol.points.size() != 0)
     {
      QFont tfont ("Mono");
      tfont.setPixelSize (8);

      painter.setFont (tfont);

      for (int i = 0; i < env_vol.points.size(); i++)
          {
           int pos_sections = env_vol.points[i]->position_frames / frames_per_section; 
           
           int point_x = pos_sections - section_from; //in sections
           int point_y = get_fvalue (image_height, env_vol.points[i]->value);
           
           QPen envpen;
           envpen.setColor (QColor (cl_envelope));
           envpen.setWidth (2);
           envpen.setStyle (Qt::DotLine);
         
           painter.setPen (envpen);
                    
           if (i < env_vol.points.size() && old_point_x != -1)          
               painter.drawLine (old_point_x, old_point_y, point_x, point_y);
  
           envpen.setStyle (Qt::SolidLine);
         
           if (env_vol.points[i]->selected)
              {
               envpen.setColor (QColor (cl_env_point_selected));
               envpen.setWidth (4);
              }
         
           painter.setPen (envpen);
 
           QRect rectangle (point_x, point_y, ENVELOPE_SIDE, ENVELOPE_SIDE); 
 
           painter.drawRect (rectangle);
           //painter.drawText (QPoint (point_x + 1, point_y + 1), QString::number (point_y));
          
           
           float v1 = conv (env_vol.points[i]->value, 50, 100);
  
            //cout << "raw: " << x << endl;
  
           float v2 = conv_to_db (v1, -50, 50, -90.0f, 6.0f);
 
           painter.drawText (QPoint (point_x + 1, point_y + 1), QString::number (v2));
          
          
           old_point_x = point_x;
           old_point_y = point_y;
          }
   }
  
  waveform_image = img;

//  qDebug() << "CWaveform::prepare_image() - end";
}


int CWaveform::get_cursor_position_sections()
{
  if (frames_per_section == 0 || fb->channels == 0)
     return 0;

  return fb->offset / frames_per_section;
}


int CWaveform::get_selection_start_sections()
{
  if (frames_per_section == 0 || fb->channels == 0)
     return 0;

  return sel_start_frames / frames_per_section;
}


int CWaveform::get_selection_end_sections()
{
  if (frames_per_section == 0 || fb->channels == 0)
     return 0;

 return sel_end_frames / frames_per_section;
}


void CWaveform::set_cursor_by_section (size_t section)
{
  fb->offset = section * frames_per_section;
}



void CWaveform::mouseDoubleClickEvent (QMouseEvent *event)
{
  select_all();
  event->accept();
}


void CWaveform::mousePressEvent (QMouseEvent *event)  
{
  if (! fb)
     {
      event->accept();
      return;
     }
  
  setFocus (Qt::OtherFocusReason);
  mouse_pressed = true;
 
  int y = event->y();

  if (y < 0)
     y = 0;  

  if (y > height())
     y = height();  


  int section = section_from + event->x();

  if (event->button() == Qt::RightButton)
     {
      //env_vol.insert_wise (section * frames_per_section, event->y(), height(), sound_buffer->samples_total - 1);
      env_vol.insert_wise (section * frames_per_section, y, height(), fb->length_frames);
  
       
      //env_vol.insert_wise (section * frames_per_section * sound_buffer->channels, event->y(), height(), 
        //                   section_to * frames_per_section * sound_buffer->channels);

    
     //qDebug() << "calc:" << section_to * frames_per_section * sound_buffer->channels;
     //qDebug() << "total:" << sound_buffer->samples_total - 1;
     
    
      setFocus (Qt::OtherFocusReason);
 
      recalc_view();
      prepare_image();
      
      update();

      previous_mouse_pos_x = section;
      return;
     }

  
  CEnvelopePoint *ep = env_vol.find (section * frames_per_section, y, height(), frames_per_section);

  if (ep)
     {
      //qDebug() << "ep->position = " << ep->position_frames;
      //qDebug() << "ep->value = " << ep->value;
      
      env_vol.select_point (ep);
      
      envelope_selected = 0;
      
      if (event->modifiers() & Qt::ShiftModifier)  
          ep->value = 50;
      
      if (event->modifiers() & Qt::ControlModifier)  
          {
           int idx = env_vol.points.indexOf (ep);
           if (idx != 0 && idx != env_vol.points.size() - 1)
              {
               delete ep;
               env_vol.points.removeAt (idx);
              } 
          }
      
  
      recalc_view();
      prepare_image();
      
      update();

      previous_mouse_pos_x = section;
      return;
     }
  


//deselect if click is not on the selection bounds
  if (! x_nearby (section, get_selection_start_sections(), 2) &&
      ! x_nearby (section, get_selection_end_sections(), 2))
      {
       deselect();
       set_cursor_value (section);
      } 
  else
      {
       //selected = true;

       if (x_nearby (section, get_selection_start_sections(), 2))
          {
           set_selstart_value (section);
           selection_selected = 1;
  //         qDebug() << "selection_selected: " << selection_selected; 
          } 
          
       else  
       if (x_nearby (section, get_selection_end_sections(), 2))
          {
           set_selend_value (section);
           selection_selected = 2;
//           qDebug() << "selection_selected: " << selection_selected; 

          }
           
      }   
  
  
  if (! selected)
      set_cursor_value (section);
    
  update();
  
  set_cursorpos_text();
  previous_mouse_pos_x = section;

}


void CWaveform::mouseMoveEvent (QMouseEvent *event)
{
  if (! fb)
     {
      event->accept();
      return;
     }

  if (mouse_pressed)
    {
     if (event->pos().x() > width())
        {
         if (scrollbar->value() + 16 != scrollbar->maximum())
            scrollbar->setValue (scrollbar->value() + 16);
     }

     if (event->pos().x() < 0)
        {
         if (scrollbar->value() - 16 != 0)
             scrollbar->setValue (scrollbar->value() - 16);
        }
    }
  
  int x = event->x();
  if (x < 0)
     x = 0;

  if (x > width())
     x = width();
 
  int y = event->y();

  if (y < 0)
     y = 0;  
  
  if (y > height())
     y = height();  


  size_t current_section = section_from + x;

    if (! mouse_pressed)
     {
      if (x_nearby (current_section, get_selection_start_sections(), 1) ||
         x_nearby (current_section, get_selection_end_sections(), 1))
        {
         setCursor (Qt::SizeHorCursor);
         normal_cursor_shape = false;
        }
     else
         if (! normal_cursor_shape)
           {
            normal_cursor_shape = true;
            setCursor(Qt::ArrowCursor);
           }
     }

 
 //захват огибающей
 
  if (mouse_pressed && selection_selected == 0 && envelope_selected != -1)
     {

      CEnvelopePoint *ep = env_vol.get_selected();

      if (ep)
         {
          env_vol.point_move (ep, current_section * frames_per_section, y, height());
  
          recalc_view();
          prepare_image();
      
          update();

         return;
        }
  }
 
   
  //условия возникновения создания выделения, если нет такового
  if (! selected)
  if (mouse_pressed)
     {
     // qDebug() << "create selection";
      if (current_section < previous_mouse_pos_x)
         {
          set_selstart_value (current_section);
          set_selend_value (previous_mouse_pos_x);
          selection_selected = 1;
         }
      else   
      if (current_section > previous_mouse_pos_x)
         {
          set_selstart_value (previous_mouse_pos_x);
          set_selend_value (current_section);
          selection_selected = 2;
         }
      
      selected = true;
      return;
     } 
   
   
  if (mouse_pressed)
     {
      if (selection_selected == 1)
          set_selstart_value (current_section);
      else
          if (selection_selected == 2)
          set_selend_value (current_section);

      fix_selection_bounds();

      set_statusbar_text();

      update();
     }

  previous_mouse_pos_x = current_section;

  QWidget::mouseMoveEvent (event);
}


void CWaveform::mouseReleaseEvent (QMouseEvent * event)
{
  if (! fb)
     {
      event->accept();
      return;
     }

  mouse_pressed = false;

  selection_selected = 0;
  envelope_selected = -1;
//  if (selected)
  //   fix_selection_bounds_release();

  QWidget::mouseReleaseEvent (event);
}


CDSP::CDSP (QObject *parent): QObject (parent)
{
  maxl = 0;
  maxr = 0;
  
  gain = 1.0f;
  pan = 0.5f;
  panner = settings->value ("panner", 0).toInt();
  
//  temp_float_buffer = 0; 
  temp_float_buffer = new CFloatBuffer (buffer_size_frames, 2);
}


CDSP::~CDSP()
{
  delete temp_float_buffer;
}
  
