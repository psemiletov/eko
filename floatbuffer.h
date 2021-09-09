#ifndef FLOATBUFFER_H
#define FLOATBUFFER_H

//VER 10

#include <cstring>

#define FLOAT_BUFFER_MAX_CHANNELS 8


class CFloatBuffer
{

public:

  float *buffer[FLOAT_BUFFER_MAX_CHANNELS];
  
  float **pbuffer;

  float *buffer_interleaved; //for mapping from buffer

  size_t samplerate;
  size_t channels;
  size_t length_frames;
  size_t offset; //of pbuffer or for some puproses
  
  //ringbuffer: tail index = head - 1
  size_t head; //read from here
  size_t tail; //write to here
  size_t ringbuffer_length; //in the range of length_frames
  
  int sndfile_format;
  
  CFloatBuffer (size_t len, size_t channels_count);
  CFloatBuffer (float *interleaved_buffer, size_t len, size_t channels_count = 1);
    
  ~CFloatBuffer();  
  
  void pbuffer_reset();
  void pbuffer_inc (size_t val);
  void settozero();
  
  void ringbuffer_head_inc();
  void ringbuffer_tail_inc();
  void ringbuffer_set_length (size_t len);

  void allocate_interleaved();
  void fill_interleaved();
  
  float *to_interleaved();
  
  CFloatBuffer* clone();
  
  CFloatBuffer* copy (size_t offset_from, size_t size); //frames

  void copy_to_pos (CFloatBuffer *other, size_t offset_from, size_t size, size_t offset_to); //frames
  void copy_channel_to_pos (CFloatBuffer *other, size_t ch_from, size_t ch_to,
                            size_t offset_from, size_t size, size_t offset_to); //frames
  
  void copy_to_pos_with_rate (CFloatBuffer *other, size_t offset_from, size_t size, size_t offset_to, float rate); //frames
  
  void overwrite_at (CFloatBuffer *other, size_t pos_frames);


  CFloatBuffer* convert_to_stereo (bool full);
  CFloatBuffer* convert_to_mono();

  CFloatBuffer* resample (size_t new_rate, int resampler = 0);

  CFloatBuffer* delete_range (size_t frames_start, size_t frames_end);

  void copy_params (CFloatBuffer *fb);
  void copy_from (CFloatBuffer *other);
  void copy_from_w_resample (CFloatBuffer *other, int resampler = 0);

  void paste_at (CFloatBuffer *other, size_t pos_frames);
};

#endif // FLOATBUFFER_H
