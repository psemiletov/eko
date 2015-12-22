#include <cstring>
#include <cmath>

#include <QDebug>

#include <samplerate.h>
#include <sndfile.h>

#include "sound_buffer.h"


//function to overwrite data in this soundbuffer from the other one,
//from the pos position

void CSoundBuffer::overwrite_at (CSoundBuffer *other, size_t pos_frames)
{
  if (! other && ! other->float_buffer)
     return;
  
  size_t frames_to_copy = other->float_buffer->length_frames;
  if (frames_to_copy > (float_buffer->length_frames - pos_frames))
    frames_to_copy = float_buffer->length_frames - pos_frames;
  
  for (size_t ch = 0; ch < other->float_buffer->channels; ch++)
      {
       memcpy (float_buffer->buffer[ch] + pos_frames, other->float_buffer->buffer[ch], frames_to_copy * sizeof (float));
      }
}


//convert this sb to stereo and return a new instance
CSoundBuffer* CSoundBuffer::convert_to_stereo (bool full)
{
  if (float_buffer->channels != 1)
     return 0;


  CFloatBuffer *tfb = new CFloatBuffer (float_buffer->length_frames, 2);
  
  memcpy (tfb->buffer[0], float_buffer->buffer[0], float_buffer->length_frames * sizeof (float));
  memcpy (tfb->buffer[1], float_buffer->buffer[1], float_buffer->length_frames * sizeof (float));
     
  if (! full)
     {
      for (size_t i = 0; i < float_buffer->length_frames; i++)
         {
          tfb->buffer[0][i] *= 0.5f;
          tfb->buffer[1][i] *= 0.5f;
         }
     }
  
  
  CSoundBuffer *nb = new CSoundBuffer (2);

  nb->float_buffer = tfb;
  nb->float_buffer->offset = 0;
  nb->samplerate = samplerate;  
  
  return nb;
}


//the same thing for the mono
//stereo channels are distributed to mono 
//with a half level from both channels
CSoundBuffer* CSoundBuffer::convert_to_mono()
{
  if (float_buffer->channels != 2)
     return 0;
  
  CFloatBuffer *fb = new CFloatBuffer (float_buffer->length_frames, 1);
  
  for (size_t i = 0; i < float_buffer->length_frames; i++)
      {
       float l = float_buffer->buffer [0][i] * 0.5;
       float r = float_buffer->buffer [0][i] * 0.5;
       fb->buffer[0][i] = l + r;     
      }
    
  CSoundBuffer *nb = new CSoundBuffer (1);
  nb->float_buffer = fb;
    
  nb->float_buffer->offset = 0;
  
  nb->sndfile_format = sndfile_format;
  nb->samplerate = samplerate;  
  
  return nb;
}


//convert this sb to stereo and return a new instance
/*
CSoundBuffer* CSoundBuffer::convert_ch6_to_stereo (int algo)
{
  if (channels != 6)
     return NULL;

  int six_channels[5];

  if (((sndfile_format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV) ||
      ((sndfile_format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAVEX) ||
      ((sndfile_format & SF_FORMAT_TYPEMASK) == SF_FORMAT_FLAC)
     )
     {
      six_channels[CH_FRONT_LEFT] = 0;
      six_channels[CH_FRONT_RIGHT] = 1;
      six_channels[CH_FRONT_CENTER] = 2;
      six_channels[CH_LOW_FREQUENCY] = 3;
      six_channels[CH_BACK_LEFT] = 4;
      six_channels[CH_BACK_RIGHT] = 5;
     }

  if ((sndfile_format & SF_FORMAT_TYPEMASK) == SF_FORMAT_AIFF)
     {
      six_channels[CH_FRONT_LEFT] = 0;
      six_channels[CH_FRONT_RIGHT] = 3;
      six_channels[CH_FRONT_CENTER] = 2;
      six_channels[CH_LOW_FREQUENCY] = 5;
      six_channels[CH_BACK_LEFT] = 1;
      six_channels[CH_BACK_RIGHT] = 4;
     }


  size_t new_samples = frames * 2;
  size_t new_buffer_size = new_samples * sizeof (float);

  float *tb = new float [new_samples];

  size_t end = samples_total - 1;
  size_t i = 0;
  size_t c = 0;

  while (i < end)
       {

        float front_left = buffer[i + six_channels[CH_FRONT_LEFT]];
        float front_right = buffer[i + six_channels[CH_FRONT_RIGHT]];
        float front_center = buffer[i + six_channels[CH_FRONT_CENTER]];
        float back_left = buffer[i + six_channels[CH_BACK_LEFT]];
        float back_right = buffer[i + six_channels[CH_BACK_RIGHT]];
        float lfe = buffer[i + six_channels[CH_LOW_FREQUENCY]];

        float rear_gain = 0.5f;
        float center_gain = 0.7f;

        float left = 0.0f;
        float right = 0.0f;

        if (algo == 0)
           {
            left = front_left + rear_gain * back_left + center_gain * front_center;
            right = front_right + rear_gain * back_right + center_gain * front_center;
           }
        else
        if (algo == 1) //Dolby Surround-like
           {
            float tmp_mono_rear = 0.7 * (back_left + back_right);
            left = front_left + rear_gain * tmp_mono_rear + center_gain * front_center;
            right = front_right + rear_gain * tmp_mono_rear + center_gain * front_center;
           }


        tb[c++] = left;
        tb[c++] = right;
        i += 6;
       }

  CSoundBuffer *nb = new CSoundBuffer;

  nb->buffer = tb;

  nb->buffer_offset = 0;
  nb->buffer_size = new_buffer_size;

  nb->channels = 2;
  nb->samples_total = new_samples;
  nb->frames = new_samples / 2;
  nb->samplerate = samplerate;

  return nb;
}
*/

/*

http://www.hydrogenaudio.org/forums/lofiversion/index.php/t44593.html

Some software decoders allow you to choose between two different 2-channel-downmix modes: Stereo and Dolby Surround:

Stereo:
- Left = front_left + rear_gain * rear_left + center_gain * center
- Right = front_right + rear_gain * rear_right + center_gain * center

Dolby Surround (Pro Logic Downmix)
- tmp_mono_rear = 0.7 * (rear_left + rear_right)
- Left = front_left + rear_gain * tmp_mono_rear + center_gain * center
- Right = front_right - rear_gain * tmp_mono_rear + center_gain * center

where rear_gain is usually around 0.5-1.0 and center_gain is almost always 0.7 (-3 dB) if I recall correctly. Notice the polarity inversion of the tmp_mono_rear when routing to the 'Right' channel. So, the rear channels are first mixed together and then fed to Left and Right with a phase difference of 180°. This way a Pro Logic Decoder can try to recover the (mono!) rear channel.

Somehow Dolby Pro Logic 2 manages to squeeze 2 rear channels into a stereo signal. In this case I'm not sure how you should downmix the signal for such a decoder -- probably like this:
- Left = front_left + rear_gain * rear_left + center_gain * center
- Right = front_right - rear_gain * rear_right + center_gain * center

*/


/*

Every file format has a different channel ordering. The following table gives this ordering for some formats (useful for plugin writers )
reference: channel 1: channel 2: channel 3: channel 4: channel 5: channel 6:
5.1 WAV front left channel front right channel front center channel LFE rear left channel rear right channel
5.1 AC3 front left channel front center channel front right channel rear left channel rear right channel LFE
5.1 DTS front center channel front left channel front right channel rear left channel rear right channel LFE
5.1 AAC front center channel front left channel front right channel rear left channel rear right channel LFE
5.1 AIFF front left channel rear left channel front center channel front right channel rear right channel LFE

* 5.1 DTS: the LFE is on a separate stream (much like on multichannel MPEG2).
* AAC specifications are unavailable on the internet (a free version
*/





//returns the resampled data as a new buffer
//the source data remains untouched
CSoundBuffer* CSoundBuffer::resample (int new_rate)
{

  float ratio = (float) 1.0 * new_rate / samplerate; 
  size_t output_frames = (size_t) floor (float_buffer->length_frames * ratio);

  CFloatBuffer *tfb = new CFloatBuffer (output_frames, float_buffer->channels);
  
  for (size_t ch = 0; ch < tfb->channels; ch++)
      {
       SRC_DATA data;
       data.src_ratio = ratio;
  
       data.input_frames = float_buffer->length_frames;
       data.output_frames = output_frames;

       //float *data_out = new float [data.output_frames * channels];
    
       data.data_in = float_buffer->buffer[ch];
       data.data_out = tfb->buffer[ch];;
 
       int error = src_simple (&data, SRC_SINC_BEST_QUALITY, 1);
       if (error)
          {
           qDebug() << src_strerror (error);
           delete tfb;
           return 0;
          }
      }
     

  CSoundBuffer *nb = new CSoundBuffer (tfb->channels);
  
  nb->sndfile_format = sndfile_format;
  nb->float_buffer = tfb;
  //nb->frames = data.output_frames;  //REAL LENGTH!
  nb->samplerate = new_rate;  
  
  return nb;
}


//delete data at the range from start to end
void CSoundBuffer::delete_range (size_t frames_start, size_t frames_end)
{
  if (! float_buffer)
     return;

  size_t len = frames_end - frames_start;
  size_t new_buffer_frames_count = float_buffer->length_frames - len;
  
  CFloatBuffer *tfb = new CFloatBuffer (new_buffer_frames_count, float_buffer->channels); 
  
  for (size_t ch = 0; ch < float_buffer->channels; ch++)
     {
      memcpy (tfb->buffer[ch], float_buffer->buffer[ch], frames_start * sizeof (float));
     }
  

  for (size_t ch = 0; ch < float_buffer->channels; ch++)
     {
      memcpy (tfb->buffer[ch] + frames_start, float_buffer->buffer[ch] + frames_start + len,
              (float_buffer->length_frames - frames_end) * sizeof (float));
     }
 

  if (float_buffer)
     delete float_buffer;
  
  float_buffer = tfb;
}


//copy parameters from "sb" to this sound buffer
void CSoundBuffer::copy_params (CSoundBuffer *sb)
{
  if (! sb)
      return;

  float_buffer->offset = sb->float_buffer->offset;
 
  samplerate = sb->samplerate;
  
  use_sndfile_format = sb->use_sndfile_format;
  sndfile_format = sb->sndfile_format;
}


//copy all data from "other" to this buffer, including the data parameters
void CSoundBuffer::copy_from (CSoundBuffer *other)
{
  if (! other || ! other->float_buffer)
    return;

  copy_params (other);

  if (float_buffer)
     delete float_buffer; 

  float_buffer = other->float_buffer->clone();
}


void CSoundBuffer::copy_from_w_resample (CSoundBuffer *other)
{
  if (! other || ! other->float_buffer)
    return;

  int f = sndfile_format;

  if (samplerate != other->samplerate) //TEST IT!
     {
      CSoundBuffer *sb = other->resample (samplerate);
      copy_from (sb);
      sndfile_format = f;
      delete sb;
      return;
     }

   copy_from (other);
   sndfile_format = f;
}


//paste from "other" to this buffer, at the "pos" position
//resample if it needs to be resampled

void CSoundBuffer::paste_at (CSoundBuffer *other, size_t pos_frames)
{
  if (! other && ! other->float_buffer)
     return;

  if (samplerate != other->samplerate) //TEST IT!
     {
      CSoundBuffer *sb = other->resample (samplerate);
      paste_at (sb, pos_frames);
      delete sb;
      return;
     }

  CSoundBuffer *temp_buffer = 0;
  
  size_t chcount;
  
  if (float_buffer)
      chcount = float_buffer->channels;
  else
      chcount = default_channels_count;    

  if (chcount == 1 && other->float_buffer->channels == 2)
     temp_buffer = other->convert_to_mono();
  else
  if (chcount == 2 && other->float_buffer->channels == 1)
      temp_buffer = other->convert_to_stereo (false);
  else
  if (chcount == other->float_buffer->channels)
     {
      temp_buffer = new CSoundBuffer (chcount);
      temp_buffer->copy_from (other);
     }


  size_t position_in_frames = pos_frames;

  size_t new_buffer_frames_count = temp_buffer->float_buffer->length_frames + float_buffer->length_frames;
  
  //создаем новый буфер, размером равный текущему со вставляемым
  CFloatBuffer *tfb = new CFloatBuffer (new_buffer_frames_count, default_channels_count);
  
  //из старого буфера в новый, копируем все по позицию position_in_frames 
  
  for (size_t ch = 0; ch < float_buffer->length_frames; ch++)
      {
       memcpy (tfb->buffer[ch], float_buffer->buffer[ch], position_in_frames * sizeof (float));
      }
    

 //перематываем новый в позицию position_in_frames и копируем в него вставляемый

  for (size_t ch = 0; ch < float_buffer->length_frames; ch++)
      {
       memcpy (tfb->buffer[ch] + position_in_frames, temp_buffer->float_buffer->buffer[ch], 
               temp_buffer->float_buffer->length_frames * sizeof (float));
      }

 //перематываем новый в позицию position_in_frames + temp_buffer->float_buffer->length_frames и
 // копируем в него остаток старого буфера

  size_t tail = float_buffer->length_frames - pos_frames;

  for (size_t ch = 0; ch < float_buffer->length_frames; ch++)
      {
       memcpy (tfb->buffer[ch] + pos_frames + temp_buffer->float_buffer->length_frames, 
                float_buffer->buffer[ch], 
                tail * sizeof (float));
      }

  if (float_buffer)
     delete float_buffer;
     
  float_buffer = tfb;   

  delete temp_buffer;
}


CSoundBuffer::CSoundBuffer (size_t def_channels_count)
{
  default_channels_count = def_channels_count;
  float_buffer = 0;
  samplerate = 0;
  use_sndfile_format = true;
  sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;
//  float_buffer_offset_frames = 0;
}
 
 
CSoundBuffer::~CSoundBuffer()
{
  if (float_buffer)
     delete float_buffer;
     
  float_buffer = 0;
}


//copy data and its properties from this soundbuffer to new soundbuffer,
//in a given range, in samples
//return the new soundbuffer
CSoundBuffer* CSoundBuffer::copy_to (size_t frames_start, size_t frames_end)
{
  CSoundBuffer *temp_buffer = new CSoundBuffer (float_buffer->channels); 
         
  temp_buffer->float_buffer = float_buffer->copy (frames_start, frames_end - frames_start);
  temp_buffer->samplerate = samplerate;
  temp_buffer->sndfile_format = sndfile_format;

  return temp_buffer;
}
