/***************************************************************************
 *   2010-2021 by Peter Semiletov                                          *
 *   peter.semiletov@gmail.com                                             *
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
#include <QElapsedTimer>

#include <math.h>
#include <samplerate.h>

#include "document.h"
#include "utils.h"
#include "gui_utils.h"

#include "logmemo.h"
#include "fxrack.h"
#include "db.h"

#define FRAMES_PER_SECT_MAX 16

enum EMultiChannelMappingWAV
{
  SPEAKER_FRONT_LEFT = 0,
  SPEAKER_FRONT_RIGHT,
  SPEAKER_FRONT_CENTER,
  SPEAKER_LOW_FREQUENCY,
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

// Глобальные переменные, используемые в редакторе
QSettings *settings;
CDSP *dsp;
QHash <QString, QString> global_palette;
CDocumentHolder *documents;
CFxRackWindow *wnd_fxrack;
SRC_STATE *resampler;
int resample_quality;
int meter_cps;
int meter_msecs_delay;
int buffer_size_frames;
bool play_l;
bool play_r;
bool bypass_mixer;

extern CFileFormats *file_formats;   // определён в другом месте
CFloatBuffer *sound_clipboard;       // буфер обмена звука
int transport_state = 0;

// Вспомогательная функция
inline bool x_nearby (int x, int pos, int width)
{
  int a = pos - width;
  int b = pos + width;
  if (x >= a && x <= b)
    return true;
  return false;
}

// ---------- Реализация классов ----------
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

// ----- CTimeRuler -----
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
  if (! waveform)
  {
    event->accept();
    return;
  }
  if (waveform->frames_per_section == 0 || ! waveform->fb)
  {
    event->accept();
    return;
  }
  QPainter painter (this);
  painter.setFont (QFont ("Mono", 6));
  painter.setBrush (background_color);
  painter.drawRect (0, 0, width(), height());

  size_t sections = waveform->get_section_to() - waveform->get_section_from();
  float sections_per_second = (float) waveform->fb->samplerate / waveform->frames_per_section;
  size_t seconds_start = waveform->get_section_from() / sections_per_second;
  size_t seconds_end = waveform->get_section_to() / sections_per_second;
  size_t seconds = seconds_end - seconds_start;
  bool measure_in_seconds = true;
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
        t = t.addSecs ((waveform->get_section_from() + x) / sections_per_second);
        painter.drawLine (p1, p2);
        painter.drawText (x + 2, 20, t.toString ("ss:zzz") + "s");
      }
    }
    else
    {
      if (x % 60 == 0)
      {
        QPoint p1 (x, 1);
        QPoint p2 (x, 12);
        size_t minutes = (waveform->fb->length_frames / waveform->fb->samplerate) / 60;
        bool larger_than_hour = minutes > 60;
        QTime t (0, 0);
        t = t.addSecs ((waveform->get_section_from() + x) / sections_per_second);
        painter.drawLine (p1, p2);
        if (! larger_than_hour)
          painter.drawText (x + 2, 20, t.toString ("mm:ss") + "m");
        else
          painter.drawText (x + 2, 20, t.toString ("hh:mm:ss") + "m");
      }
    }
  }
  event->accept();
}

// ----- CWaveform -----
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
  int x = section - get_section_from();
  painter.drawLine (x, 1, x, height());
  if (selected)
    if (get_selection_start_sections() != get_selection_end_sections())
    {
      int s_start = get_selection_start_sections() - get_section_from();
      int s_end = get_selection_end_sections() - get_section_from();
      QPoint p1 (s_start, 0);
      QPoint p2 (s_end, height());
      QRect r (p1, p2);
      painter.fillRect (r, cl_waveform_selection_foreground);
    }
    event->accept();
}

void CWaveform::timer_timeout()
{
  int cursor_pos_sections = get_cursor_position_sections();
  if (cursor_pos_sections >= (int) get_section_to())
    scrollbar->setValue (scrollbar->value() + width());
  if (scrollbar->value() >= cursor_pos_sections)
    scrollbar->setValue (get_selection_start_sections());
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
  timer.setInterval (50);
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
  for (auto el: undos)
    delete el;
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
  if (! fb)
    return;
  sections_total = width() * scale_factor;
  if (sections_total == 0)
    return;
  frames_per_section = ceil (fb->length_frames / sections_total);
  if (frames_per_section < FRAMES_PER_SECT_MAX)
    frames_per_section = FRAMES_PER_SECT_MAX;
  scrollbar->setMinimum (0);
  scrollbar->setMaximum (sections_total - width());
}

void CWaveform::zoom (int factor)
{
  if (! fb || frames_per_section == 0)
    return;
  int old_frame_from = get_section_from() * frames_per_section;
  scale_factor = factor;
  if ((width() * scale_factor) >= fb->length_frames - 1)
    return;
  recalc_view();
  prepare_image();
  int new_section = old_frame_from / frames_per_section;
  scrollbar->setValue (new_section);
  update();
  timeruler->update();
}

void CWaveform::scale (int delta)
{
  if (! fb || frames_per_section == 0)
    return;
  int old_frame_from = get_section_from() * frames_per_section;
  if (delta > 0)
    scale_factor += 0.1f;
  else
    scale_factor -= 0.1f;
  if (scale_factor < 1.0f)
    scale_factor = 1.0f;
  if ((width() * scale_factor) >= fb->length_frames - 1)
    return;
  recalc_view();
  prepare_image();
  int new_section = old_frame_from / frames_per_section;
  scrollbar->setValue (new_section);
  update();
  timeruler->update();
}

void CWaveform::wheelEvent (QWheelEvent *e)
{
  #if QT_VERSION < 0x050000
  const int delta = e->delta();
  #else
  const int delta = e->angleDelta().y();
  #endif
  scale (delta);
  e->accept();
}

void CWaveform::resizeEvent (QResizeEvent *event)
{
  recalc_view();
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
    set_statusbar_text();
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
    set_statusbar_text();
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
    set_statusbar_text();
    event->accept();
    return;
  }
  if (event->key() == Qt::Key_Left)
  {
    if (get_cursor_position_sections() != 0)
      set_cursor_by_section (get_cursor_position_sections() - 1);
    if (get_cursor_position_sections() == (int) get_section_from())
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
    set_statusbar_text();
    event->accept();
    return;
  }
  if (event->key() == Qt::Key_Right)
  {
    set_cursor_by_section (get_cursor_position_sections() + 1);
    if (get_cursor_position_sections() == (int) sections_total)
      set_cursor_by_section (get_cursor_position_sections() - 1);
    if (get_cursor_position_sections() == (int) get_section_to())
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
      if (get_selection_end_sections() != (int) sections_total)
      {
        set_selend_value (get_selection_end_sections() + 1);
        fix_selection_bounds();
      }
      selected = true;
    }
    update();
    set_cursorpos_text();
    set_statusbar_text();
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
  if (event->text() == "[")
  {
    set_selstart_value (get_cursor_position_sections());
    fix_selection_bounds();
    selected = true;
    update();
    event->accept();
    set_cursorpos_text();
    set_statusbar_text();
    return;
  }
  if (event->text() == "]")
  {
    set_selend_value (get_cursor_position_sections());
    fix_selection_bounds();
    selected = true;
    update();
    set_cursorpos_text();
    set_statusbar_text();
    event->accept();
    return;
  }
  QWidget::keyPressEvent (event);
}

size_t CWaveform::frames_start()
{
  if (! fb)
    return 0;
  if (! selected)
    return 0;
  return sel_start_frames;
}

size_t CWaveform::frames_end()
{
  if (! fb)
    return 0;
  if (! selected)
    return fb->length_frames;
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
  if ((frames_end() - frames_start() > fb->length_frames))
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
}

void CWaveform::select_all()
{
  sel_start_frames = 0;
  sel_end_frames = fb->length_frames;
  selected = true;
  update();
}

void CWaveform::undo_take_shot (int type, int param)
{
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
  if (type == UNDO_WHOLE)
    el->fb = fb->copy (0, fb->length_frames);
  if (type == UNDO_DELETE)
    el->fb = fb->copy (frames_start(), frames_end() - frames_start());
  if (type == UNDO_MODIFY)
    el->fb = fb->copy (frames_start(), frames_end() - frames_start());
  if (type == UNDO_PASTE)
  {
    el->start_frames = fb->offset;
    el->end_frames = el->start_frames + sound_clipboard->length_frames;
  }
  if (type == UNDO_INSERT)
  {
    el->start_frames = fb->offset;
    el->end_frames = param;
  }
  undos.prepend (el);
}

void CWaveform::delete_selected()
{
  if (! selected)
    return;
  if (frames_end() - frames_start() > fb->length_frames)
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
  // пустая заглушка
}

void CWaveform::undo_top()
{
  if (undos.count() == 0)
    return;
  CUndoElement *el = undos.at (0);
  if (el->type == UNDO_UNDEFINED)
    return;
  if (el->type == UNDO_WHOLE)
  {
    delete fb;
    fb = el->fb->copy (0, el->fb->length_frames);
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
    fb->offset = el->cursor_frames;
  prepare_image();
  timeruler->update();
  update();
  undos.removeAt (0);
  delete el;
}

void CWaveform::set_cursor_to_frames (size_t frame)
{
  if (frame <= fb->length_frames)
    fb->offset = frame;
}

void CWaveform::set_statusbar_text()
{
  if (frames_per_section == 0)
    return;
  QString txt;
  int framesstart = frames_start();
  int framesend = frames_end();
  float msecs_selstart = (float) framesstart / fb->samplerate * 1000;
  float msecs_selend = (float) framesend / fb->samplerate * 1000;
  float diff = msecs_selend - msecs_selstart;
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
}

void CWaveform::set_cursorpos_text()
{
  if (! fb)
    return;
  size_t frames = fb->offset;
  float mseconds = (float) frames / fb->samplerate * 1000;
  QTime t (0, 0);
  t = t.addMSecs ((int)mseconds);
  wave_edit->doc->holder->l_maintime->setText (t.toString ("hh:mm:ss.zzz"));
}

// ----- CWaveEdit -----
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

void CWaveEdit::dropEvent (QDropEvent *event)
{
  QString fName;
  QFileInfo info;
  if (! event->mimeData()->hasUrls())
    return;
  for (const auto &u: event->mimeData()->urls())
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
  waveform->init_state = false;
  timeruler->init_state = false;
  waveform->prepare_image();
  waveform->update();
  timeruler->update();
}

// ----- CDocument -----
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
  CTio *tio = holder->tio_handler.get_for_fname (fileName);
  if (! tio)
  {
    holder->log->log (tr ("file type of %1 is not supported").arg (fileName));
    return false;
  }
  QElapsedTimer tm;
  tm.start();
  CFloatBuffer *ff = tio->load (fileName);
  if (! ff)
  {
    holder->log->log (tr ("cannot open %1 because of: %2").arg (fileName, tio->error_string));
    return false;
  }
  if (wave_edit->waveform->fb)
    delete wave_edit->waveform->fb;
  wave_edit->waveform->fb = ff;
  ronly = false;
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
  wave_edit->waveform->init_state = false;
  wave_edit->timeruler->init_state = false;
  wave_edit->waveform->magic_update();
  wave_edit->waveform->autoScaleToFit();
  holder->log->log (tr ("elapsed: %1 ms").arg (tm.elapsed()));
  return true;
}

bool CDocument::save_with_name (const QString &fileName)
{
  int fmt = wave_edit->waveform->fb->sndfile_format & SF_FORMAT_TYPEMASK;
  QString fname = fileName;
  QString ext = file_get_ext (fileName);
  QString fext = file_formats->hextensions.value (fmt);
  if (ext.isEmpty())
    fname = fname + "." + fext;
  else
    fname = change_file_ext (fname, fext);
  CTio *tio = holder->tio_handler.get_for_fname (fname);
  if (! tio)
    return false;
  tio->float_input_buffer = wave_edit->waveform->fb;
  QElapsedTimer tm;
  tm.start();
  if (! tio->save (fname))
  {
    holder->log->log (tr ("cannot save %1 because of: %2").arg (fname, tio->error_string));
    return false;
  }
  int elapsed = tm.elapsed();
  file_name = fname;
  set_tab_caption (QFileInfo (file_name).fileName());
  holder->log->log (tr ("%1 is saved").arg (file_name));
  holder->log->log (tr ("elapsed: %1 milliseconds").arg (elapsed));
  update_title();
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
  if (! tio->save (fileName))
  {
    holder->log->log (tr ("cannot save %1 because of: %2").arg (fileName, tio->error_string));
    return false;
  }
  return true;
}

// ----- CDocumentHolder -----
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
  return doc;
}

CDocument* CDocumentHolder::open_file (const QString &fileName, bool set_fname)
{
  if (! file_exists (fileName))
    return NULL;
  if (! tio_handler.is_ext_supported (fileName))
    return NULL;
  for (auto *d: list)
  {
    if (d->file_name == fileName)
    {
      tab_widget->setCurrentIndex (tab_widget->indexOf (d->tab_page));
      return d;
    }
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
  for (auto *d: list)
    apply_settings_single (d);
}

void CDocumentHolder::add_to_recent (CDocument *d)
{
  if (! file_exists (d->file_name))
    return;
  recent_files.prepend (d->file_name);
  if (recent_files.size() > 17)
    recent_files.removeLast();
}

void CDocumentHolder::update_recent_menu()
{
  recent_menu->clear();
  create_menu_from_list (this, recent_menu, recent_files, SLOT(open_recent()));
}

void CDocumentHolder::open_recent()
{
  QAction *a = qobject_cast<QAction *>(sender());
  int i = recent_files.indexOf (a->data().toString());
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
  for (auto *d: list)
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
  for (const auto &t: l)
    open_file (t);
  fname_current_session = fileName;
}

void CDocumentHolder::load_palette (const QString &fileName)
{
  def_palette = fileName;
  for (auto *d: list)
    d->wave_edit->waveform->load_color (fileName);
}

CDocumentHolder::CDocumentHolder (QObject *parent): QObject (parent)
{
  sound_clipboard = new CFloatBuffer (1, settings->value ("def_channels", 1).toInt());
  current = 0;
}

// ----- CTransportControl -----
void CTransportControl::call_play_pause()
{
  emit play_pause();
}

void CTransportControl::call_stop()
{
  emit stop();
}

// ===================== ИСПРАВЛЕННЫЕ ФУНКЦИИ =====================
void CWaveform::set_cursor_value (size_t section)
{
  if (!fb) return;
  size_t new_offset = section * frames_per_section;
  if (new_offset > fb->length_frames)
    new_offset = fb->length_frames;
  fb->offset = new_offset;
}

void CWaveform::set_selstart_value (size_t section)
{
  if (!fb) return;
  size_t new_start = section * frames_per_section;
  if (new_start > fb->length_frames)
    new_start = fb->length_frames;
  sel_start_frames = new_start;
  if (sel_start_frames > sel_end_frames)
    sel_start_frames = sel_end_frames;
}

void CWaveform::set_selend_value (size_t section)
{
  if (!fb) return;
  size_t new_end = section * frames_per_section;
  if (new_end > fb->length_frames)
    new_end = fb->length_frames;
  sel_end_frames = new_end;
  if (sel_end_frames < sel_start_frames)
    sel_end_frames = sel_start_frames;
}

// ----- CFxRackWindow -----
void CFxRackWindow::tm_level_meter_timeout()
{
  if (! level_meter)
    return;
  level_meter->update();
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
  if (value == 0)
    dsp->gain = 1.0f;
  else
    dsp->gain = db2lin (value);
}

CFxRackWindow::CFxRackWindow()
{
  QHBoxLayout *h_mainbox = new QHBoxLayout;
  setLayout (h_mainbox);
  QVBoxLayout *v_box = new QVBoxLayout;
  h_mainbox->addLayout (v_box);
  level_meter = new CVLevelMeter (this);
  bt_apply = new QPushButton (tr ("Apply"));
  fx_rack = new CFxRack;
  v_box->addWidget (fx_rack->inserts);
  v_box->addWidget (bt_apply);
  QVBoxLayout *v_meter = new QVBoxLayout;
  v_meter->addWidget (level_meter);
  cb_l.setText ("L");
  cb_r.setText ("R");
  cb_l.setChecked (true);
  cb_r.setChecked (true);
  connect(&cb_l, SIGNAL(stateChanged(int)), this, SLOT(cb_l_changed(int)));
  connect(&cb_r, SIGNAL(stateChanged(int)), this, SLOT(cb_r_changed(int)));
  QHBoxLayout *h_lr = new QHBoxLayout;
  h_lr->addWidget (&cb_l);
  h_lr->addWidget (&cb_r);
  v_meter->addLayout (h_lr);
  h_mainbox->addLayout (v_meter);
  connect(&tm_level_meter, SIGNAL(timeout()), this, SLOT(tm_level_meter_timeout()));
  tm_level_meter.setInterval (60);
  QVBoxLayout *v_vol = new QVBoxLayout;
  QLabel *l_vol = new QLabel (tr ("Volume"));
  QDial *dial_volume = new QDial;
  dial_volume->setWrapping (false);
  connect (dial_volume, SIGNAL(valueChanged(int)), this, SLOT(dial_gain_valueChanged(int)));
  dial_volume->setRange (-26, 26);
  v_vol->addWidget (l_vol);
  v_vol->addWidget (dial_volume);
  QHBoxLayout *h_volpan = new QHBoxLayout;
  h_volpan->addLayout (v_vol);
  v_box->addLayout (h_volpan);
  setWindowTitle (tr ("Mixer"));
}

void CFxRackWindow::closeEvent (QCloseEvent *event)
{
  event->accept();
}

// ----- CDSP -----
size_t CDSP::process (CDocument *d, size_t nframes)
{
  if (nframes == 0 || !d || transport_state == STATE_EXIT)
    return 0;
  maxl = 0.0f;
  maxr = 0.0f;
  size_t frames = nframes;
  if (! d->wave_edit->waveform->play_looped)
  {
    size_t tail = (d->wave_edit->waveform->fb->length_frames - d->wave_edit->waveform->fb->offset) - frames;
    if (tail < nframes)
      frames = tail;
    d->wave_edit->waveform->fb->copy_to_pos (temp_float_buffer, d->wave_edit->waveform->fb->offset, frames, 0);
    d->wave_edit->waveform->fb->offset += frames;
  }
  else
  {
    size_t diff = d->wave_edit->waveform->frames_end() - d->wave_edit->waveform->fb->offset;
    if (diff < frames)
    {
      d->wave_edit->waveform->fb->copy_to_pos (temp_float_buffer, d->wave_edit->waveform->fb->offset, diff, 0);
      d->wave_edit->waveform->fb->offset = d->wave_edit->waveform->frames_start();
      size_t part = frames - diff;
      d->wave_edit->waveform->fb->copy_to_pos (temp_float_buffer, d->wave_edit->waveform->fb->offset, part, diff);
      d->wave_edit->waveform->fb->offset += part;
    }
    else
    {
      d->wave_edit->waveform->fb->copy_to_pos (temp_float_buffer, d->wave_edit->waveform->fb->offset, frames, 0);
      d->wave_edit->waveform->fb->offset += frames;
    }
  }
  if (! bypass_mixer)
    for (int i = 0; i < wnd_fxrack->fx_rack->effects.count(); i++)
    {
      if (! wnd_fxrack->fx_rack->effects[i]->bypass)
      {
        wnd_fxrack->fx_rack->effects[i]->realtime = true;
        wnd_fxrack->fx_rack->effects[i]->channels = d->wave_edit->waveform->fb->channels;
        wnd_fxrack->fx_rack->effects[i]->samplerate = d->wave_edit->waveform->fb->samplerate;
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
    return frames;
}

size_t CDSP::process_rec (float **buffer, size_t channels, size_t nframes)
{
  if (nframes == 0 || transport_state == STATE_EXIT)
    return 0;
  maxl = 0.0f;
  maxr = 0.0f;
  if (channels == 1)
    for (size_t i = 0; i < nframes; i++)
    {
      if (float_greater_than (buffer[0][i], maxl))
        maxl = buffer[0][i];
    }
    if (channels == 2)
      for (size_t i = 0; i < nframes; i++)
      {
        if (float_less_than (maxl, buffer[0][i]))
          maxl = buffer[0][i];
        if (float_less_than (maxr, buffer[1][i]))
          maxr = buffer[1][i];
      }
      if (wnd_fxrack->level_meter)
      {
        wnd_fxrack->level_meter->pl = maxl;
        wnd_fxrack->level_meter->pr = maxr;
      }
      return nframes;
}

CDSP::CDSP (QObject *parent): QObject (parent)
{
  maxl = 0;
  maxr = 0;
  gain = 1.0f;
  temp_float_buffer = new CFloatBuffer (buffer_size_frames, 2);
}

CDSP::~CDSP()
{
  delete temp_float_buffer;
}

// ----- Дополнительные методы CWaveform (некоторые были в конце оригинала) -----
size_t CWaveform::get_section_from()
{
  return scrollbar->value();
}

size_t CWaveform::get_section_to()
{
  return width() + scrollbar->value();
}

void CWaveform::scalef (float factor, size_t start_frm)
{
  if (! fb || frames_per_section == 0)
    return;
  int old_frame_from = start_frm;
  scale_factor = factor;
  if (scale_factor < 1.0f)
    scale_factor = 1.0f;
  prepare_image();
  int new_section = old_frame_from / frames_per_section;
  scrollbar->setValue (new_section);
  update();
  timeruler->update();
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
  #if QT_VERSION < 0x060000
  int y = event->y();
  #else
  int y = event->position().y();
  #endif
  if (y < 0) y = 0;
  if (y > height()) y = height();

  #if QT_VERSION < 0x060000
  int section = get_section_from() + event->x();
  #else
  int section = get_section_from() + event->position().x();
  #endif
  // Ограничение секции максимальным значением
  size_t max_section = (fb->length_frames + frames_per_section - 1) / frames_per_section;
  if (section > (int)max_section)
    section = (int)max_section;

  if (event->button() == Qt::RightButton)
  {
    env_vol.insert_wise (section * frames_per_section, y, height(), fb->length_frames);
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
  if (! x_nearby (section, get_selection_start_sections(), 2) &&
    ! x_nearby (section, get_selection_end_sections(), 2))
  {
    deselect();
    set_cursor_value (section);
  }
  else
  {
    if (x_nearby (section, get_selection_start_sections(), 2))
    {
      set_selstart_value (section);
      selection_selected = 1;
    }
    else if (x_nearby (section, get_selection_end_sections(), 2))
    {
      set_selend_value (section);
      selection_selected = 2;
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
  #if QT_VERSION < 0x060000
  int x = event->x();
  #else
  int x = event->position().x();
  #endif
  if (x < 0) x = 0;
  if (x > width()) x = width();
  #if QT_VERSION < 0x060000
  int y = event->y();
  #else
  int y = event->position().y();
  #endif
  if (y < 0) y = 0;
  if (y > height()) y = height();

  size_t current_section = get_section_from() + x;
  size_t max_section = (fb->length_frames + frames_per_section - 1) / frames_per_section;
  if (current_section > max_section)
    current_section = max_section;

  if (! mouse_pressed)
  {
    if (x_nearby (current_section, get_selection_start_sections(), 1) ||
      x_nearby (current_section, get_selection_end_sections(), 1))
    {
      setCursor (Qt::SizeHorCursor);
      normal_cursor_shape = false;
    }
    else
    {
      if (! normal_cursor_shape)
      {
        normal_cursor_shape = true;
        setCursor(Qt::ArrowCursor);
      }
    }
  }
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
  if (! selected && mouse_pressed)
  {
    if ((int) current_section < previous_mouse_pos_x)
    {
      set_selstart_value (current_section);
      set_selend_value (previous_mouse_pos_x);
      selection_selected = 1;
    }
    else if ((int) current_section > previous_mouse_pos_x)
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
    else if (selection_selected == 2)
      set_selend_value (current_section);
    fix_selection_bounds();
    set_statusbar_text();
    update();
  }
  previous_mouse_pos_x = current_section;
  QWidget::mouseMoveEvent (event);
}

void CWaveform::mouseReleaseEvent (QMouseEvent *event)
{
  if (! fb)
  {
    event->accept();
    return;
  }
  mouse_pressed = false;
  selection_selected = 0;
  envelope_selected = -1;
  QWidget::mouseReleaseEvent (event);
}

// ===================== ИСПРАВЛЕННАЯ prepare_image =====================
void CWaveform::prepare_image()
{
  if (! fb)
  {
    qDebug() << "! sound_buffer";
    return;
  }
  if (frames_per_section == 0)
    return;

  size_t sections = get_section_to() - get_section_from();
  int image_height = height();
  int channel_height = image_height / fb->channels;

  if (minmaxes)
    delete minmaxes;
  minmaxes = new CMinmaxes (fb->channels, sections);

  size_t total_frames = fb->length_frames;
  size_t section = 0;
  size_t frameno = get_section_from() * frames_per_section;

  // Инициализация временных значений для первой секции
  for (size_t ch = 0; ch < fb->channels; ch++)
  {
    minmaxes->values[ch]->min_values->temp = 1.0f;   // больше любого sample
    minmaxes->values[ch]->max_values->temp = -1.0f;  // меньше любого sample
  }

  // Основной цикл по секциям, но не выходим за границы буфера
  while (section < sections && frameno < total_frames)
  {
    for (size_t ch = 0; ch < fb->channels; ch++)
    {
      float sample = fb->buffer[ch][frameno];
      if (sample < minmaxes->values[ch]->min_values->temp)
        minmaxes->values[ch]->min_values->temp = sample;
      if (sample > minmaxes->values[ch]->max_values->temp)
        minmaxes->values[ch]->max_values->temp = sample;
    }
    frameno++;

    // Если достигли конца секции или конца файла
    if ((frameno % frames_per_section == 0) || (frameno == total_frames))
    {
      for (size_t ch = 0; ch < fb->channels; ch++)
      {
        // Если в секции не было данных (frameno достигло конца файла)
        if (minmaxes->values[ch]->min_values->temp > 0.5f)
          minmaxes->values[ch]->min_values->samples[section] = 0.0f;
        else
          minmaxes->values[ch]->min_values->samples[section] = minmaxes->values[ch]->min_values->temp;

        if (minmaxes->values[ch]->max_values->temp < -0.5f)
          minmaxes->values[ch]->max_values->samples[section] = 0.0f;
        else
          minmaxes->values[ch]->max_values->samples[section] = minmaxes->values[ch]->max_values->temp;

        // Сброс временных значений для следующей секции
        minmaxes->values[ch]->min_values->temp = 1.0f;
        minmaxes->values[ch]->max_values->temp = -1.0f;
      }
      section++;
    }
  }

  // Заполняем нулями оставшиеся секции (если файл закончился раньше)
  while (section < sections)
  {
    for (size_t ch = 0; ch < fb->channels; ch++)
    {
      minmaxes->values[ch]->min_values->samples[section] = 0.0f;
      minmaxes->values[ch]->max_values->samples[section] = 0.0f;
    }
    section++;
  }

  // Рисование изображения волновой формы
  QImage img (width(), image_height, QImage::Format_RGB32);
  QPainter painter (&img);
  painter.setPen (cl_waveform_foreground);
  img.fill (cl_waveform_background.rgb());

  if (fb->length_frames == 32)
  {
    painter.drawLine (QPoint (0, 0), QPoint (width(), height()));
    painter.drawLine (QPoint (0, height()), QPoint (width(), 0));
    waveform_image = img;
    return;
  }

  // Рисуем амплитудные столбики
  for (size_t ch = 0; ch < fb->channels; ch++)
    for (size_t x = 0; x < sections; x++)
    {
      int ymax = channel_height - (int)((minmaxes->values[ch]->max_values->samples[x] + 1) * 0.5f * channel_height);
      int ymin = channel_height - (int)((minmaxes->values[ch]->min_values->samples[x] + 1) * 0.5f * channel_height);
      QPoint p1 (x, ymin + channel_height * ch);
      QPoint p2 (x, ymax + channel_height * ch);
      painter.drawLine (p1, p2);
    }

    // Рисуем ось и шкалу dB
    painter.setPen (cl_text);
    QFont tfont ("Mono");
    tfont.setPixelSize (8);
    painter.setFont (tfont);
    for (size_t ch = 0; ch < fb->channels; ch++)
    {
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
        painter.drawText (QPoint (1, y), QString::number (db, 'g', 2));
        if (db >= -9)
          db++;
        else
          db += 5;
      }
    }

    // Рисуем огибающую громкости
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
        int point_x = pos_sections - get_section_from();
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
        float v1 = conv (env_vol.points[i]->value, 50, 100);
        float v2 = conv_to_db (v1, -50, 50, -90.0f, 6.0f);
        painter.drawText (QPoint (point_x + 1, point_y + 1), QString::number (v2) + "dB");
        old_point_x = point_x;
        old_point_y = point_y;
      }
    }
    waveform_image = img;
}

void CWaveform::autoScaleToFit()
{
  if (!fb || width() <= 0) return;

  int w = width();
  size_t total_frames = fb->length_frames;

  // Минимальное количество фреймов, которое может отобразить виджет при стандартной детализации
  // FRAMES_PER_SECT_MAX = 16, т.е. каждая секция = 16 фреймов
  size_t min_frames_for_normal = w * FRAMES_PER_SECT_MAX;

  if (total_frames < min_frames_for_normal)
  {
    // Короткий файл: растягиваем на всю ширину
    // Рассчитываем frames_per_section так, чтобы общее количество секций = width()
    size_t target_frames_per_section = (total_frames + w - 1) / w;
    if (target_frames_per_section < 1) target_frames_per_section = 1;

    frames_per_section = target_frames_per_section;
    scale_factor = 1.0f;               // количество секций = width()
    sections_total = w;                // = width() * scale_factor
    scrollbar->setMinimum(0);
    scrollbar->setMaximum(sections_total - w);
    scrollbar->setValue(0);

    prepare_image();
    update();
    timeruler->update();
  }
  else
  {
    // Длинный файл: просто сбрасываем прокрутку в начало,
    // но не трогаем frames_per_section и scale_factor (они уже корректно заданы в recalc_view)
    scrollbar->setValue(0);
    // Перерисовка не требуется, так как magic_update уже вызвал prepare_image,
    // но если хотим гарантировать, можно сделать update()
    update();
    timeruler->update();
  }
}
