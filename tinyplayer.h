#ifndef TINYPLAYER_H
#define TINYPLAYER_H

#include <QString>
#include <sndfile.h>


class CTinyPlayer: public QObject
{
  Q_OBJECT

public:

  QString filename;

  float *buffer; //temp buffer
  size_t buffer_offset;
  size_t buffer_size; //the size in bytes of *buffer
  size_t samples_total; //total samples at *buffer
  size_t frames; //frames at buffer
  
  size_t file_size; //samples
  
  int channels;
  int samplerate;
  
  SNDFILE *file; 
  SF_INFO sf;
  
  float* get_next_buffer_portion();

  bool load (const QString &fname);
  
  void play();
  void stop();
  void pause();
  
  CTinyPlayer (const QString &fname, size_t length); //in samples, = system buffer size * 10
  ~CTinyPlayer();

};



#endif // TINYPLAYER_H
