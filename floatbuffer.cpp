//VER 8

#include <iostream>
#include <math.h>

#include <sndfile.h>
#include <samplerate.h>

#include "floatbuffer.h"


CFloatBuffer::CFloatBuffer (size_t len, size_t channels_count)
{
  head = 1;
  tail = 0;
  
  ringbuffer_length = len;

  pbuffer = 0;
  channels = channels_count;
  length_frames = len;
  buffer_interleaved = 0;
  samplerate = 0;
  sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;   
  
  if (channels > FLOAT_BUFFER_MAX_CHANNELS)
     channels = FLOAT_BUFFER_MAX_CHANNELS;
  
  for (size_t ch = 0; ch < channels; ch++)
      {
       buffer[ch] = new float [length_frames];
       memset (buffer[ch], 0, length_frames * sizeof (float));
      } 
}


CFloatBuffer::CFloatBuffer (float *interleaved_buffer, size_t len)
{
  head = 1;
  tail = 0;
  ringbuffer_length = len;

  pbuffer = 0;
  
  channels = 1;
  length_frames = len;
  buffer_interleaved = 0;
  samplerate = 0;
  sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;  
  buffer[0] = interleaved_buffer;
}
 
 
CFloatBuffer::CFloatBuffer (float *interleaved_buffer, size_t len, size_t channels_count)
{
  head = 1;
  tail = 0;
  ringbuffer_length = len;

  pbuffer = 0;
  
  channels = channels_count;
  length_frames = len;
  buffer_interleaved = 0;
  samplerate = 0;
  sndfile_format = 0;
  sndfile_format = sndfile_format | SF_FORMAT_WAV | SF_FORMAT_FLOAT;  
   
  if (channels > FLOAT_BUFFER_MAX_CHANNELS)
     channels = FLOAT_BUFFER_MAX_CHANNELS;
  
  for (size_t ch = 0; ch < channels; ch++)
      {
       buffer[ch] = new float [length_frames];
       memset (buffer[ch], 0, length_frames * sizeof (float));
      } 

   size_t c = 0;   
      
   for (size_t i = 0; i < length_frames; i++)
      {
       for (size_t ch = 0; ch < channels; ch++) 
           {
            buffer[ch][i] = interleaved_buffer[c++];
           }     
      }
}
  

CFloatBuffer::~CFloatBuffer()
{
  for (size_t ch = 0; ch < channels; ch++)
      delete [] buffer[ch];
      
  if (buffer_interleaved)    
     delete [] buffer_interleaved;
}  


float* CFloatBuffer::to_interleaved()
{
  size_t buflen = length_frames * channels;
    
  float *interleaved_buffer = new float [buflen];

   size_t c = 0;   
      
   for (size_t i = 0; i < length_frames; i++)
      {
       for (size_t ch = 0; ch < channels; ch++) 
           {
            interleaved_buffer[c++] = buffer[ch][i];
           }     
      }

  return interleaved_buffer;
}


CFloatBuffer* CFloatBuffer::clone()
{
  CFloatBuffer *fb = new CFloatBuffer (length_frames, channels);

  for (size_t ch = 0; ch < channels; ch++) 
      {
       memcpy (fb->buffer[ch], buffer[ch], length_frames * sizeof (float));
      }     

  fb->copy_params (this);
  
  return fb;
}


CFloatBuffer* CFloatBuffer::copy (size_t offset_from, size_t size)
{
  if (size > length_frames)
     return 0;

  if (offset_from > length_frames)
     return 0;

  if (size > (length_frames - offset_from))
     return 0;

  CFloatBuffer *fb = new CFloatBuffer (size, channels);

  fb->copy_params (this);
  fb->length_frames = size;

  for (size_t ch = 0; ch < channels; ch++) 
      {
       memcpy (fb->buffer[ch], buffer[ch] + offset_from, size * sizeof (float));
      }     

  return fb;
}

void CFloatBuffer::copy_to_pos (CFloatBuffer *other, size_t offset_from, size_t size, size_t offset_to)
{
  if (size > length_frames)
     return;
      
  if (offset_from > length_frames)
     return;

  size_t copysize = size;
  size_t reminder = length_frames - offset_from;

  if (reminder < size)
     copysize = reminder;
  
  for (size_t ch = 0; ch < channels; ch++) 
      {
       memcpy (other->buffer[ch] + offset_to, buffer[ch] + offset_from, copysize * sizeof (float));
      }     
}


void CFloatBuffer::copy_channel_to_pos (CFloatBuffer *other, size_t ch_from, size_t ch_to,
                                        size_t offset_from, size_t size, size_t offset_to)
{
  if (size > length_frames)
     return;
      
  if (offset_from > length_frames)
     return;

  size_t reminder = length_frames - offset_from;
  if (reminder < size)
     return;
  
   memcpy (other->buffer[ch_to] + offset_to, buffer[ch_from] + offset_from, size * sizeof (float));
}


void CFloatBuffer::copy_to_pos_with_rate (CFloatBuffer *other, size_t offset_from, size_t size, size_t offset_to, float rate)
{
  if (size > length_frames)
     return;

  if (offset_from > length_frames)
     return;

  size_t reminder = length_frames - offset_from;
  if (reminder < size)
     return;
  
  for (size_t ch = 0; ch < channels; ch++) 
      {
       size_t c = 0;
       size_t i = 0;
             
       float *p_dest_buffer = other->buffer[ch] + offset_to;
       float *p_source_buffer = buffer[ch] + offset_from;      
             
       while (i < size)
             {
              p_dest_buffer[c++] = p_source_buffer[(size_t)floor (i * rate)];
              i++;
             }
      }          
}


void CFloatBuffer::pbuffer_reset()
{
  offset = 0;
  
  if (pbuffer)
     delete pbuffer;
  
  pbuffer = new float* [channels];
  
   for (size_t ch = 0; ch < channels; ch++)
       {
        pbuffer[ch] = buffer[ch];
       }

}


void CFloatBuffer::pbuffer_inc (size_t val)
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       pbuffer[ch] += val;
      }
      
  offset += val;
}


void CFloatBuffer::settozero()
{
  for (size_t ch = 0; ch < channels; ch++)
      {
       memset (buffer[ch], 0, length_frames * sizeof (float));  
      }
}


void CFloatBuffer::allocate_interleaved()
{
  if (buffer_interleaved)
     delete [] buffer_interleaved;   
  
  buffer_interleaved = new float [length_frames * channels];
  memset (buffer_interleaved, 0, length_frames * channels * sizeof (float));
}
 
 
void CFloatBuffer::fill_interleaved()
{
   size_t c = 0;   
      
   for (size_t i = 0; i < length_frames; i++)
      {
       for (size_t ch = 0; ch < channels; ch++) 
           {
            buffer_interleaved[c++] = buffer[ch][i];
           }     
      }
}

 
//function to overwrite data in this soundbuffer from the other one,
//from the pos position
void CFloatBuffer::overwrite_at (CFloatBuffer *other, size_t pos_frames)
{
  if (! other)
     return;
  
  size_t frames_to_copy = other->length_frames;
  size_t reminder = length_frames - pos_frames;
  
  if (frames_to_copy > reminder)
      frames_to_copy = reminder;
  
  for (size_t ch = 0; ch < other->channels; ch++)
      {
       memcpy (buffer[ch] + pos_frames, other->buffer[ch], frames_to_copy * sizeof (float));
      }
}


//convert this sb to stereo and return a new instance
CFloatBuffer* CFloatBuffer::convert_to_stereo (bool full)
{
  if (channels != 1)
     return 0;

  CFloatBuffer *tfb = new CFloatBuffer (length_frames, 2);
  
  memcpy (tfb->buffer[0], buffer[0], length_frames * sizeof (float));
  memcpy (tfb->buffer[1], buffer[0], length_frames * sizeof (float));
     
  if (! full)
     {
      for (size_t i = 0; i < length_frames; i++)
         {
          tfb->buffer[0][i] *= 0.5f;
          tfb->buffer[1][i] *= 0.5f;
         }
     }
   
  tfb->copy_params (this);
  tfb->channels = 2;
  
  return tfb;
}


//the same thing for the mono
//stereo channels are distributed to mono 
//with a half level from both channels
CFloatBuffer* CFloatBuffer::convert_to_mono()
{
  if (channels != 2)
     return 0;
  
  CFloatBuffer *tfb = new CFloatBuffer (length_frames, 1);
  
  for (size_t i = 0; i < length_frames; i++)
      {
       float l = buffer [0][i] * 0.5;
       float r = buffer [0][i] * 0.5;
       tfb->buffer[0][i] = l + r;     
      }
  
  tfb->copy_params (this);
  tfb->channels = 1;
  
  return tfb;
}


//returns the resampled data as a new buffer
//the source data remains untouched
CFloatBuffer* CFloatBuffer::resample (size_t new_rate)
{
  float ratio = (float) 1.0f * new_rate / samplerate; 
  size_t output_frames = (size_t) floor (length_frames * ratio);

  CFloatBuffer *tfb = new CFloatBuffer (output_frames, channels);
  
  for (size_t ch = 0; ch < tfb->channels; ch++)
      {
       SRC_DATA data;
       data.src_ratio = ratio;
  
       data.input_frames = length_frames;
       data.output_frames = output_frames;
    
       data.data_in = buffer[ch];
       data.data_out = tfb->buffer[ch];;
 
       int error = src_simple (&data, SRC_SINC_BEST_QUALITY, 1);
       if (error)
          {
           delete tfb;
           return 0;
          }
      }
     
  
  //nb->frames = data.output_frames;  //REAL LENGTH!
  tfb->copy_params (this);
  tfb->length_frames = output_frames;
  tfb->samplerate = new_rate;
  
  return tfb;
}


//delete data at the range from start to end
CFloatBuffer* CFloatBuffer::delete_range (size_t frames_start, size_t frames_end)
{
  size_t delrange = frames_end - frames_start;
  size_t new_buffer_frames_count = length_frames - delrange;
  
  CFloatBuffer *tfb = new CFloatBuffer (new_buffer_frames_count, channels); 
  
  for (size_t ch = 0; ch < channels; ch++)
     {
      memcpy (tfb->buffer[ch], buffer[ch], frames_start * sizeof (float));
     }

  for (size_t ch = 0; ch < channels; ch++)
     {
      memcpy (tfb->buffer[ch] + frames_start, buffer[ch] + frames_start + delrange,
             (length_frames - frames_end) * sizeof (float));
     }
 
  tfb->copy_params (this);
  tfb->offset = frames_start;
  tfb->length_frames = new_buffer_frames_count;
  
  return tfb;
}

//copy parameters from "fb" to this sound buffer
void CFloatBuffer::copy_params (CFloatBuffer *fb)
{
  if (! fb)
      return;

  offset = fb->offset;
  samplerate = fb->samplerate;
  sndfile_format = fb->sndfile_format;
  length_frames = fb->length_frames;
  channels = fb->channels;
}


//copy all data from "other" to this buffer, including the data parameters
void CFloatBuffer::copy_from (CFloatBuffer *other)
{
  if (! other)
    return;

  for (size_t ch = 0; ch < channels; ch++)
      {
       delete [] buffer[ch]; 
      } 

  copy_params (other);

  for (size_t ch = 0; ch < channels; ch++)
      {
       buffer[ch] = new float [length_frames];
      } 

  other->copy_to_pos (this, 0, other->length_frames, 0);
}


void CFloatBuffer::copy_from_w_resample (CFloatBuffer  *other)
{
  if (! other || ! other->buffer[0])
    return;

  if (samplerate != other->samplerate) //TEST IT!
     {
      CFloatBuffer *fb = other->resample (samplerate);
      copy_from (fb);
      delete fb;
      return;
     }

  copy_from (other);
}


//paste from "other" to this buffer, at the "pos" position
//resample if it needs to be resampled

void CFloatBuffer::paste_at (CFloatBuffer *other, size_t pos_frames)
{
  if (! other)
     return;

  size_t pos_frames_corrected = pos_frames;
  if (pos_frames > length_frames)
     pos_frames_corrected = length_frames;

  if (samplerate != other->samplerate) //TEST IT!
     {
      //qDebug() << "TEST IT!";
      CFloatBuffer *fb = other->resample (samplerate);
      paste_at (fb, pos_frames_corrected);
      delete fb;
      return;
     }
 
  CFloatBuffer *temp_buffer = 0;
    
  if (channels == 1 && other->channels == 2)
     temp_buffer = other->convert_to_mono();
  else
  if (channels == 2 && other->channels == 1)
      temp_buffer = other->convert_to_stereo (false);
  else
  if (channels == other->channels)
     temp_buffer = other->clone();
 

  size_t new_buffer_frames_count = temp_buffer->length_frames + length_frames;
  
  //создаем новый буфер, размером равный текущему со вставляемым
  CFloatBuffer *sum = new CFloatBuffer (new_buffer_frames_count, channels);
  sum->samplerate = samplerate;  

  //из старого буфера в новый, копируем все по позицию position_in_frames 
  for (size_t ch = 0; ch < channels; ch++)
      {
       memcpy (sum->buffer[ch], buffer[ch], pos_frames_corrected * sizeof (float));
      }
  

 //перематываем новый в позицию position_in_frames и копируем в него вставляемый

  for (size_t ch = 0; ch < channels; ch++)
      {
       memcpy (sum->buffer[ch] + pos_frames_corrected, temp_buffer->buffer[ch], 
               temp_buffer->length_frames * sizeof (float));
      }

 //перематываем новый в позицию position_in_frames + temp_buffer->float_buffer->length_frames и
 // копируем в него остаток старого буфера

  size_t tail = length_frames - pos_frames_corrected;

  cout << "tail: " << tail <<  endl;
  cout  << "pos_frames: " << pos_frames_corrected << endl;
  //qDebug() << "temp_buffer->length_frames: " << pos_frames;
  

  for (size_t ch = 0; ch < channels; ch++)
      {
       memcpy (sum->buffer[ch] + pos_frames_corrected + temp_buffer->length_frames, 
               buffer[ch] + pos_frames_corrected, 
               tail * sizeof (float));
      }
          
  copy_from (sum);   

  delete sum;
  delete temp_buffer;
  
  offset = pos_frames_corrected;
}


void CFloatBuffer::ringbuffer_head_inc()
{
  head++;
  
  if (head >= ringbuffer_length)
     head = 0;
}

void CFloatBuffer::ringbuffer_tail_inc()
{
  tail++;
  
  if (tail >= ringbuffer_length)
     tail = 0;
}


void CFloatBuffer::ringbuffer_set_length (size_t len)
{
  ringbuffer_length = len;
  
  if (ringbuffer_length > length_frames)
     ringbuffer_length = length_frames;   
}
