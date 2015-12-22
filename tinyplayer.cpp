#include "tinyplayer.h"



bool CTinyPlayer::load (const QString &fname)
{


}
  
  
void CTinyPlayer::play()
{

}
 

void CTinyPlayer::stop()
{


}



void CTinyPlayer::pause()
{


}
  
  
CTinyPlayer::CTinyPlayer (const QString &fname, size_t length)
{
  buffer = new float [buffer_size];

  buffer_offset = 0;
  buffer_size = length * sizeof (float); //the size in bytes of *buffer
  samples_total = length; //total samples at *buffer
  
  channels = 0;
  samplerate = 0;
  
  
  sf.format = 0;
  
  file = sf_open (fname.toUtf8().data(), SFM_READ, &sf);
  
  file_size = sf.channels * sf.frames; 
  
  
  

}


CTinyPlayer::~CTinyPlayer()
{
  if (buffer)
     delete buffer;


  sf_close (file);
  
}


float* CTinyPlayer::get_next_buffer_portion()
{

//  float *output_data  = new float [total_samples];
//  sf_count_t zzz = sf_readf_float (file, output_data, sf.frames);

  
  if (! output_data)
    {
     qDebug() << "! output_data";
     return NULL; 
    }  
  
  frames = sf.frames;
  samplerate = sf.samplerate;
  channels = sf.channels;
  format = sf.format;


}
