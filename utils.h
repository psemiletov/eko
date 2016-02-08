/*
this code is Public Domain
Peter Semiletov
*/

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QHash>
#include <QFileInfo>
#include <QStringList>

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


QString hash_keyval_to_string (const QHash<QString, QString> &h);
QString hash_get_val (QHash<QString, QString> &h, const QString &key, const QString &def_val);

QString file_get_ext (const QString &file_name);

bool file_exists (const QString &fileName);
void qstring_list_print (const QStringList &l);

bool qstring_save (const QString &fileName, const QString &data, const char *enc = "UTF-8");
QString qstring_load (const QString &fileName, const char *enc = "UTF-8");

QMap <QString, QString> map_load_keyval (const QString &fname);
QString map_keyval_to_string (const QMap <QString, QString> &h);


QStringList read_dir_entries (const QString &path);
QHash<QString, QString> hash_load (const QString &fname);
QHash<QString, QString> hash_load_keyval (const QString &fname);
QHash<QString, QString> stringlist_to_hash (const QStringList &l);
QString hash_keyval_to_string (QHash<QString, QString> *h);
bool is_image (const QString &filename);
QString string_between (const QString &source, const QString &sep1, const QString &sep2);
QByteArray file_load (const QString &fileName);


QString change_file_ext (const QString &s, const QString &ext);
//QString get_insert_image (const QString &file_name, const QString &full_path, const QString &markup_mode);



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


double input_double_value (const QString &caption, const QString &lbl,
                           double minval, double maxval, double defval, double step);



size_t round_to (size_t value, size_t to, bool inc);



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


QString str_from_locale (const char *s);


inline float scale_val (float val, float from_min, float from_max, float to_min, float to_max)
{
  return (val - from_min) * (to_max - to_min) / 
          (from_max - from_min) + to_min;
}

#endif
