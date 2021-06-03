/*
this code is Public Domain
Peter Semiletov
*/

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QAction>
#include <QTime>

#include <cmath>
#include <limits>


class CFilesList: public QObject
{
public:

  QStringList list;

  void get (const QString &path);
  void iterate (QFileInfo &fi);
};


class CFTypeChecker: public QObject
{
public:

  QStringList lexts;
  QStringList lnames;

  CFTypeChecker (const QString &fnames, const QString &exts);
  bool check (const QString &fname);
};


bool is_dir (const QString &path);


bool qstring_save (const QString &fileName, const QString &data, const char *enc = "UTF-8");
QString qstring_load (const QString &fileName, const char *enc = "UTF-8");

QString string_between (const QString &source, const QString &sep1, const QString &sep2);
QString str_from_locale (const char *s);


float get_value_with_default (const QString &val, float def);
size_t get_value_with_default (const QString &val, size_t def);
int get_value_with_default (const QString &val, int def);
QString get_value_with_default (const QString &val, const QString &def);

QString hash_keyval_to_string (const QHash<QString, QString> &h);
QString hash_get_val (QHash<QString, QString> &h, const QString &key, const QString &def_val);

QMap <QString, QString> map_load_keyval (const QString &fname, const QString &sep);
QString map_keyval_to_string (const QMap <QString, QString> &h, const QString &sep);

QHash<QString, QString> hash_load (const QString &fname);
QHash<QString, QString> hash_load_keyval (const QString &fname);
QHash<QString, QString> stringlist_to_hash (const QStringList &l);


QByteArray file_load (const QString &fileName);

QString file_get_ext (const QString &file_name);
QString change_file_ext (const QString &s, const QString &ext);

bool file_exists (const QString &fileName);

QStringList read_dir_entries (const QString &path);
QStringList read_dir_files (const QString &path);

void qstring_list_print (const QStringList &l);
bool is_image (const QString &filename);

size_t round_to (size_t value, size_t to, bool inc);


inline int get_value (int total, int perc)
{
  return static_cast <int> (total * perc / 100);
}


inline float get_fvalue (float total, float perc)
{
  return (total * perc / 100);
}


inline double get_percent (double total, double value)
{
  return (value / total) * 100;
}


inline float get_percent (float total, float value)
{
  return (value / total) * 100;
}


inline bool is_dir (const QString &path)
{
  return QFileInfo(path).isDir();
}


inline QString get_file_path (const QString &fileName)
{
  return QFileInfo (fileName).absolutePath();
}


inline bool float_greater_than (float a, float b)
{
  return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<double>::epsilon());
}


inline bool float_less_than (float a, float b)
{
  return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<double>::epsilon());
}


inline bool float_equal (float x, float y)
{
  return std::abs(x - y) <= std::numeric_limits<double>::epsilon() * std::abs(x);
}


inline float conv (float v, float middle, float max)
{
  if (v == middle)
     return 0;

  if (v > middle)
     return (max - middle - v);
  else
      return middle - v;
}


inline float conv_to_db (float v, float v_min, float v_max, float range_negative, float range_positive)
{
  if (v == 0)
     return 0;

  if (v > 0)
    {
     float x = v_max / range_positive;
     float y = v_max / v;

     return v / (y * x);
    }
  else
      {
       float x = v_min / range_negative;
       float y = v_min / v;

       return v / (y * x);
      }
}


inline float scale_val (float val, float from_min, float from_max, float to_min, float to_max)
{
  return (val - from_min) * (to_max - to_min) /
          (from_max - from_min) + to_min;
}


inline QString frames_to_time_str (size_t frames, size_t samplerate)
{
  size_t msecs = (float) frames / samplerate * 1000;

  QTime a (0, 0);
  a = a.addMSecs ((int) msecs);

 return a.toString ("hh:mm:ss.zzz");
}


inline size_t msecs_to_frames (size_t msecs, size_t samplerate)
{
  return samplerate * msecs / 1000;
}


inline QTime frames_to_time (size_t frames, size_t samplerate)
{
  size_t msecs = (float) frames / samplerate * 1000;
  QTime a (0, 0);
  a = a.addMSecs ((int) msecs);
  return a;
}

#endif
