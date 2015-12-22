#ifndef TIO_H
#define TIO_H

#include <QObject>
#include <QMultiHash>
#include <QHash>
#include <QStringList>


#include <sndfile.h>

#include "floatbuffer.h"



class CFileFormats
{
public:

  QMultiHash <int, int> hformat;
  QHash <int, QString> hsubtype;
  QHash <int, QString> hformatnames;
  QHash <int, QString> hextensions;

  CFileFormats();
};


class CSignaturesList: public QObject
{
  Q_OBJECT

public:
  QString encname;
  QList <QByteArray> words;
};


class CTio: public QObject
{
  Q_OBJECT

public:

  QString id;

  bool ronly;
  
 // int samplerate;
 // int format;
  
  CFloatBuffer *float_input_buffer;
  QStringList extensions;
  QString error_string;

  virtual CFloatBuffer* load (const QString &fname) = 0;
  virtual bool save (const QString &fname) = 0;
  virtual bool save_16bit_pcm (const QString &fname) = 0;
};


class CTioPlainAudio: public CTio
{
  Q_OBJECT

public:

  CTioPlainAudio();
  CTioPlainAudio (bool rnly);

  ~CTioPlainAudio();
  CFloatBuffer* load (const QString &fname);
  bool save (const QString &fname);
  bool save_16bit_pcm (const QString &fname);
};


class CTioProxy: public CTio
{
  Q_OBJECT

public:

  QHash <QString, QString> proxies;

  CTioProxy();
  ~CTioProxy();
  CFloatBuffer* load (const QString &fname);
  bool save (const QString &fname);
  bool save_16bit_pcm (const QString &fname);
};



class CTioReadOnly: public CTio
{
  Q_OBJECT

public:

  bool save (const QString &fname);
};


class CTioHandler: public QObject
{
  Q_OBJECT

public:

  QList <CTio *> list;
  
  QStringList supported_extensions;
  
  CTioHandler();
  ~CTioHandler();

  bool is_ext_supported (const QString &fname);

  CTio* get_for_fname (const QString &fname);
};


void file_formats_init();
void file_formats_done();

float* load_from_lame (QString &fname);


#endif // TIO_H
