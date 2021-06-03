/*
this code is Public Domain
Peter Semiletov
*/


#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QImageReader>
#include <QImage>
#include <QTextCodec>
#include <QByteArray>
#include <QMap>

#include "gui_utils.h"
#include "utils.h"


QString qstring_load (const QString &fileName, const char *enc)
{
  QFile file (fileName);

  if (! file.open (QFile::ReadOnly | QFile::Text))
     return QString();

  QTextStream in(&file);
  //in.setCodec (enc);

  return in.readAll();
}


bool qstring_save (const QString &fileName, const QString &data, const char *enc)
{
  QFile file (fileName);
  if (! file.open (QFile::WriteOnly | QFile::Text))
      return false;

  QTextStream out (&file);
  //out.setCodec (enc);
  out << data;

  return true;
}


QString string_between (const QString &source,
                        const QString &sep1,
                        const QString &sep2)
{
  QString result;
  int pos1 = source.indexOf (sep1);
  if (pos1 == -1)
     return result;

  int pos2 = source.indexOf (sep2, pos1 + sep1.size());
  if (pos2 == -1)
     return result;

  pos1 += sep1.size();

  result = source.mid (pos1, pos2 - pos1);
  return result;
}


QString str_from_locale (const char *s)
{
  QTextCodec *cd = QTextCodec::codecForLocale();
  QByteArray encodedString = s;

  return cd->toUnicode(encodedString);
}


QString get_value_with_default (const QString &val, const QString &def)
{
  if (! val.isEmpty())
    return val;
  else
      return def;
}


int get_value_with_default (const QString &val, int def)
{
  if (! val.isEmpty())
    return val.toInt();
  else
      return def;
}


size_t get_value_with_default (const QString &val, size_t def)
{
  if (! val.isEmpty())
    return (size_t) val.toInt();
  else
      return def;
}


float get_value_with_default (const QString &val, float def)
{
  if (! val.isEmpty())
    return val.toFloat();
  else
      return def;
}


QString hash_keyval_to_string (const QHash <QString, QString> &h)
{
  QStringList l;

  foreach (QString s, h.keys())
          l.prepend (s.append ("=").append (h.value (s)));

  return l.join ("\n").trimmed();
}


QString map_keyval_to_string (const QMap <QString, QString> &h, const QString &sep)
{
  QStringList l;

  foreach (QString s, h.keys())
          l.prepend (s.append (sep).append (h.value (s)));

  return l.join ("\n").trimmed();
}


QString hash_get_val (QHash<QString, QString> &h,
                      const QString &key,
                      const QString &def_val)
{
  QString result = h.value (key);
  if (result.isNull() || result.isEmpty())
     {
      result = def_val;
      h.insert (key, def_val);
     }

  return result;
}


QHash <QString, QString> stringlist_to_hash (const QStringList &l)
{
  QHash <QString, QString> result;

  if (l.empty())
     return result;

  foreach (QString s, l)
          result.insert (s, s);

  return result;
}


QHash <QString, QString> hash_load (const QString &fname)
{
  QHash<QString, QString> result;

  if (! file_exists (fname))
     return result;

  result = stringlist_to_hash (qstring_load (fname).split ("\n"));
  return result;
}


QHash <QString, QString> hash_load_keyval (const QString &fname)
{
  QHash <QString, QString> result;

  if (! file_exists (fname))
     return result;

  QStringList l = qstring_load (fname).split ("\n");

  foreach (QString s, l)
          {
           QStringList sl = s.split ("=");
           if (sl.size() > 1)
               result.insert (sl[0], sl[1]);
          }

  return result;
}


QMap <QString, QString> map_load_keyval (const QString &fname, const QString &sep)
{
  QMap <QString, QString> result;

  if (! file_exists (fname))
     return result;

  QStringList l = qstring_load (fname).split ("\n");

  foreach (QString s, l)
          {
           QStringList sl = s.split (sep);
           if (sl.size() > 1)
               result.insert (sl[0], sl[1]);
          }

  return result;
}


QByteArray file_load (const QString &fileName)
{
  QFile file (fileName);
  QByteArray b;

  if (! file.open (QFile::ReadOnly))
      return b;

  b = file.readAll();
  return b;
}


QString file_get_ext (const QString &file_name)
{
  if (file_name.isNull() || file_name.isEmpty())
      return QString();

  int i = file_name.lastIndexOf (".");
  if (i == -1)
      return QString();

  return file_name.mid (i + 1).toLower();
}


QString change_file_ext (const QString &s, const QString &ext)
{
  int i = s.lastIndexOf (".");
  if (i == -1)
     return (s + "." + ext);


  QString r (s);
  r.truncate (++i);

  r.append (ext);
  return r;
}


bool file_exists (const QString &fileName)
{
  if (fileName.isNull() || fileName.isEmpty())
     return false;

  return QFile::exists (fileName);
}


QStringList read_dir_entries (const QString &path)
{
  QDir dir (path);
  return dir.entryList (QDir::AllEntries | QDir::NoDotAndDotDot);
}


QStringList read_dir_files (const QString &path)
{
  QDir dir (path);
  return dir.entryList (QDir::Files | QDir::NoDotAndDotDot);
}
void CFilesList::iterate (QFileInfo &fi)
{
  if (fi.isDir())
     {
      QDir d (fi.absoluteFilePath());
      QFileInfoList lfi= d.entryInfoList (QDir::Dirs | QDir::Files |
                                          QDir::Readable | QDir::NoDotAndDotDot);
      for (int i = 0; i < lfi.count(); i++)
           iterate (lfi[i]);
     }
  else
  if (fi.isFile())
      list.append (fi.absoluteFilePath());
}


void CFilesList::get (const QString &path)
{
  list.clear();
  QDir d (path);
  QFileInfoList lfi= d.entryInfoList (QDir::Dirs | QDir::Files |
                                      QDir::Readable | QDir::NoDotAndDotDot);
  for (int i = 0; i < lfi.count(); i++)
      iterate (lfi[i]);
}


CFTypeChecker::CFTypeChecker (const QString &fnames, const QString &exts)
{
  lexts = qstring_load (exts).split ("\n");
  lnames = qstring_load (fnames).split ("\n");
}


bool CFTypeChecker::check (const QString &fname)
{
  bool result = lnames.contains (fname.toLower());
  if (! result)
     {
      QString ext = file_get_ext (fname);
      result = lexts.contains (ext.toLower());
     }

  return result;
}


size_t round_to (size_t value, size_t to, bool inc)
{
   int i = value;

   if (inc)
      {
       if (i % to != 0)
          i = (i / to) * to + to;
      }
   else
       if (i % to != 0)
          i = (i / to) * to;

  return i;
}


void qstring_list_print (const QStringList &l)
{
  foreach (QString s, l)
          qDebug() << s;
}

/*
bool is_dir (const QString &path)
{
  QDir d (path);
  return d.exists();
}
*/
