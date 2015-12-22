#ifndef SOUND_BUFFER_H
#define SOUND_BUFFER_H

#include "floatbuffer.h"

class CSoundBuffer
{
  
public:

  bool use_sndfile_format;
  int sndfile_format;
  int samplerate;
  
  size_t default_channels_count;

  //size_t float_buffer_offset_frames;
    
  CFloatBuffer *float_buffer;
    
  CSoundBuffer (size_t def_channels_count);
  ~CSoundBuffer();

  void copy_from (CSoundBuffer *other);
  void copy_from_w_resample (CSoundBuffer *other);

  CSoundBuffer* copy_to (size_t frame_start, size_t frame_end);
  void overwrite_at (CSoundBuffer *other, size_t pos_frames);
  
  void delete_range (size_t frames_start, size_t frames_end);
  void paste_at (CSoundBuffer *other, size_t pos_frames);
  
  CSoundBuffer* resample (int new_rate);
  CSoundBuffer* convert_to_mono();
  CSoundBuffer* convert_to_stereo (bool full);
  //CSoundBuffer* convert_ch6_to_stereo (int algo = 0);

  void copy_params (CSoundBuffer *sb);
};


#endif // SOUND_BUFFER_H
