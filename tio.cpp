#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QTime>
#include <QCoreApplication>

#include <sndfile.h>
#include <limits>

#include "tio.h"
#include "utils.h"


using namespace std;


int proxy_video_decoder;
double ogg_q;
CFileFormats *file_formats;
QString temp_mp3_fname;


float* load_whole_sample (const char *fname, SF_INFO &sf)
{
  SNDFILE *file = sf_open (fname, SFM_READ, &sf);
  float *buffer = new float [sf.channels * sf.frames];
  sf_count_t zzz = sf_readf_float (file, buffer, sf.frames);
  sf_close (file);

  return buffer;
}


CTioPlainAudio::~CTioPlainAudio()
{
 // if (data)
  //   free (data);
}


CFloatBuffer* CTioPlainAudio::load (const QString &fname)
{
  qDebug() << "CTioPlainAudio::load - 1";
  SF_INFO sf;
  sf.format = 0;
  
  SNDFILE *file = sf_open (fname.toUtf8().data(), SFM_READ, &sf);
    
  float *filebuf = new float [sf.channels * sf.frames];
  sf_count_t zzz = sf_readf_float (file, filebuf, sf.frames);
    
  sf_close (file);
  
  if (! filebuf)
    {
     qDebug() << "! output_data";
     return NULL; 
    }  
  
  //samplerate = sf.samplerate;
//  format = sf.format;
    
  CFloatBuffer *fb = 0;
  
  if (sf.channels == 1)
     fb = new CFloatBuffer (filebuf, sf.frames);
  else   
      fb = new CFloatBuffer (filebuf, sf.frames, sf.channels);  
    
  if (sf.channels != 1)
     delete [] filebuf;
  
  fb->samplerate = sf.samplerate;
  fb->sndfile_format = sf.format;

//  qDebug() << "frames = " << sf.frames;
  //qDebug() << "total_samples = " << total_samples;
  //qDebug() << "bytes = " << total_samples * sizeof (float);
  //qDebug() << "MAX_INT = " << numeric_limits<int>::max();


  //qDebug() << "CTioPlainAudio::load - 2";
  

  return fb;
}


bool CTioPlainAudio::save (const QString &fname)
{
  if (! float_input_buffer)
     {
      qDebug() << "! data";
      return false;
     } 

  SF_INFO sf;

/*
  sf.samplerate = samplerate;
  sf.channels = float_input_buffer->channels;
  sf.format = format;
  */
  
  sf.samplerate = float_input_buffer->samplerate;
  sf.channels = float_input_buffer->channels;
  sf.format = float_input_buffer->sndfile_format;
  
  
  if (! sf_format_check (&sf))
     {
      qDebug() << "! sf_format_check (&sf)";
      return false;  
     }  
  
  //if pcm 16 bit
  if ((sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_16) 
      return save_16bit_pcm (fname);

  SNDFILE *hfile = sf_open (fname.toUtf8().data(), SFM_WRITE, &sf);

  double quality = ogg_q;
  sf_command (hfile, SFC_SET_VBR_ENCODING_QUALITY, &quality, sizeof (quality)) ;

  //qDebug() << "ogg_q = " << ogg_q;

  if (float_input_buffer->channels == 1)
     {
      sf_writef_float (hfile, float_input_buffer->buffer[0], float_input_buffer->length_frames);
     }
  else
      {
       float *interleavedbuf = float_input_buffer->to_interleaved();
       sf_writef_float (hfile, interleavedbuf, float_input_buffer->length_frames);
       delete [] interleavedbuf;
      } 
  
  
  sf_close (hfile);

  return true;
}


CTioHandler::CTioHandler()
{
  list.append (new CTioPlainAudio);
  list.append (new CTioProxy);
 
  CTio *instance;

  for (int i = 0; i < list.size(); i++)
      {
       instance = list.at (i);
       supported_extensions += instance->extensions;
      }
}


CTioHandler::~CTioHandler()
{
  while (! list.isEmpty())
       delete list.takeFirst();
}


bool CTioHandler::is_ext_supported (const QString &fname)
{
  QString ext = file_get_ext (fname).toLower();

  if (supported_extensions.indexOf (ext) != -1)
     return true;

  return false;
}


CTio* CTioHandler::get_for_fname (const QString &fname)
{
  CTio *instance;
  QString ext = file_get_ext (fname).toLower();
  
  for (int i = 0; i < list.size(); i++)
      {
       instance = list.at (i);
       
       if (instance->extensions.indexOf (ext) != -1)
          return instance;
      }

  return NULL;
}


CTioPlainAudio::CTioPlainAudio()
{
  id = "CTioPlainAudio";
  ronly = false;
  
  //samplerate = 0;
  //format = 0;
  
  extensions += file_formats->hextensions.values();
}


CTioPlainAudio::CTioPlainAudio (bool rnly)
{
  ronly = rnly;
  float_input_buffer = 0;
  //samplerate = 0;
  //format = 0;
}


bool CTioReadOnly::save (const QString &fname)
{
  error_string = tr ("saving of this format is not supported");
  return false;
}


CFileFormats::CFileFormats()
{
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_IMA_ADPCM);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_MS_ADPCM);
  hformat.insert (SF_FORMAT_WAV, SF_FORMAT_GSM610);

  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_IMA_ADPCM);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_MS_ADPCM);
  hformat.insert (SF_FORMAT_WAVEX, SF_FORMAT_GSM610);
  
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_GSM610);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DWVW_12);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DWVW_16);
  hformat.insert (SF_FORMAT_AIFF, SF_FORMAT_DWVW_24);
  
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_G721_32);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_G723_24);
  hformat.insert (SF_FORMAT_AU, SF_FORMAT_G723_40);
  
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_GSM610);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DWVW_12);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DWVW_16);
  hformat.insert (SF_FORMAT_RAW, SF_FORMAT_DWVW_24);
  
  hformat.insert (SF_FORMAT_PAF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_PAF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_PAF, SF_FORMAT_PCM_24);
    
  hformat.insert (SF_FORMAT_SVX, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_SVX, SF_FORMAT_PCM_16);
  
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_NIST, SF_FORMAT_ALAW);
  
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_IRCAM, SF_FORMAT_ALAW);

  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_VOC, SF_FORMAT_ALAW);
  
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_ALAW);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_IMA_ADPCM);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_MS_ADPCM);
  hformat.insert (SF_FORMAT_W64, SF_FORMAT_GSM610);
  
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_MAT4, SF_FORMAT_DOUBLE);
  
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_PCM_U8);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_MAT5, SF_FORMAT_DOUBLE);
  
  hformat.insert (SF_FORMAT_PVF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_PVF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_PVF, SF_FORMAT_PCM_32);
  
  hformat.insert (SF_FORMAT_XI, SF_FORMAT_DPCM_8);
  hformat.insert (SF_FORMAT_XI, SF_FORMAT_DPCM_16);
  
  hformat.insert (SF_FORMAT_HTK, SF_FORMAT_PCM_16);
  
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_24);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_PCM_32);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_FLOAT);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_DOUBLE);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_ULAW);
  hformat.insert (SF_FORMAT_CAF, SF_FORMAT_ALAW);
  
  hformat.insert (SF_FORMAT_SD2, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_SD2, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_SD2, SF_FORMAT_PCM_24);
  
  hformat.insert (SF_FORMAT_FLAC, SF_FORMAT_PCM_S8);
  hformat.insert (SF_FORMAT_FLAC, SF_FORMAT_PCM_16);
  hformat.insert (SF_FORMAT_FLAC, SF_FORMAT_PCM_24);


  hformat.insert (SF_FORMAT_OGG, SF_FORMAT_VORBIS);


  hformatnames.insert (SF_FORMAT_WAV, "Microsoft WAV format (little endian)");
  hformatnames.insert (SF_FORMAT_AIFF, "Apple/SGI AIFF format (big endian)");
  hformatnames.insert (SF_FORMAT_AU, "Sun/NeXT AU format (big endian)");
  hformatnames.insert (SF_FORMAT_RAW, "RAW PCM data");
  hformatnames.insert (SF_FORMAT_PAF, "Ensoniq PARIS file format");
  hformatnames.insert (SF_FORMAT_SVX, "Amiga IFF / SVX8 / SV16 format");
  hformatnames.insert (SF_FORMAT_NIST, "Sphere NIST format");
  hformatnames.insert (SF_FORMAT_VOC, "VOC files");
  hformatnames.insert (SF_FORMAT_IRCAM, "Berkeley/IRCAM/CARL");
  hformatnames.insert (SF_FORMAT_W64, "Sonic Foundry's 64 bit RIFF/WAV");
  hformatnames.insert (SF_FORMAT_MAT4, "Matlab (tm) V4.2 / GNU Octave 2.0");
  hformatnames.insert (SF_FORMAT_MAT5, "Matlab (tm) V5.0 / GNU Octave 2.1");
  hformatnames.insert (SF_FORMAT_PVF, "Portable Voice Format");
  hformatnames.insert (SF_FORMAT_XI, "Fasttracker 2 Extended Instrument");
  hformatnames.insert (SF_FORMAT_HTK, "HMM Tool Kit format");
  hformatnames.insert (SF_FORMAT_SDS, "Midi Sample Dump Standard");
  hformatnames.insert (SF_FORMAT_AVR, "Audio Visual Research");
  hformatnames.insert (SF_FORMAT_WAVEX, "MS WAVE with WAVEFORMATEX");
  hformatnames.insert (SF_FORMAT_SD2, "Sound Designer 2");
  hformatnames.insert (SF_FORMAT_FLAC, "FLAC lossless file format");
  hformatnames.insert (SF_FORMAT_CAF, "Core Audio File format");
  hformatnames.insert (SF_FORMAT_OGG, "Xiph OGG container");
    

  hextensions.insert (SF_FORMAT_WAV, "wav");
  hextensions.insertMulti (SF_FORMAT_AIFF, "aiff");
  hextensions.insertMulti (SF_FORMAT_AIFF, "aif");
  hextensions.insert (SF_FORMAT_AU, "au");
  hextensions.insert (SF_FORMAT_RAW, "raw");
  hextensions.insert (SF_FORMAT_PAF, "paf");
  hextensions.insert (SF_FORMAT_SVX, "iff");
  hextensions.insert (SF_FORMAT_NIST, "Sphere NIST format");//ext?
  hextensions.insert (SF_FORMAT_VOC, "voc");
  hextensions.insert (SF_FORMAT_IRCAM, "sf"); //not sound font
  hextensions.insert (SF_FORMAT_W64, "wav");
  hextensions.insert (SF_FORMAT_MAT4, "mat");
  hextensions.insert (SF_FORMAT_MAT5, "mat");
  hextensions.insert (SF_FORMAT_PVF, "pvf");
  hextensions.insert (SF_FORMAT_XI, "xi");
  hextensions.insert (SF_FORMAT_HTK, "htk");
  hextensions.insert (SF_FORMAT_SDS, "sds");
  hextensions.insert (SF_FORMAT_AVR, "avr");
  hextensions.insert (SF_FORMAT_WAVEX, "wav"); //see http://msdn.microsoft.com/en-us/library/dd757720%28VS.85%29.aspx
  hextensions.insert (SF_FORMAT_SD2, "sd2");
  hextensions.insert (SF_FORMAT_FLAC, "flac");
  hextensions.insert (SF_FORMAT_CAF, "caf");
  hextensions.insert (SF_FORMAT_OGG, "ogg");
      
  
  hsubtype.insert (SF_FORMAT_PCM_S8, "Signed 8 bit data");
  hsubtype.insert (SF_FORMAT_PCM_16, "Signed 16 bit data");
  hsubtype.insert (SF_FORMAT_PCM_24, "Signed 24 bit data");
  hsubtype.insert (SF_FORMAT_PCM_32, "Signed 32 bit data");
  hsubtype.insert (SF_FORMAT_PCM_U8, "Unsigned 8 bit data (WAV and RAW only)");
  hsubtype.insert (SF_FORMAT_FLOAT, "32 bit float data");
  hsubtype.insert (SF_FORMAT_DOUBLE, "64 bit float data");
  hsubtype.insert (SF_FORMAT_ULAW, "U-Law encoded");
  hsubtype.insert (SF_FORMAT_ALAW, "A-Law encoded");
  hsubtype.insert (SF_FORMAT_IMA_ADPCM, "IMA ADPCM");
  hsubtype.insert (SF_FORMAT_MS_ADPCM, "Microsoft ADPCM");
  hsubtype.insert (SF_FORMAT_GSM610, "GSM 6.10 encoding");
  hsubtype.insert (SF_FORMAT_VOX_ADPCM, "Oki Dialogic ADPCM encoding");
  hsubtype.insert (SF_FORMAT_G721_32, "32kbs G721 ADPCM encoding");
  hsubtype.insert (SF_FORMAT_G723_24, "24kbs G723 ADPCM encoding");
  hsubtype.insert (SF_FORMAT_G723_40, "40kbs G723 ADPCM encoding");
  hsubtype.insert (SF_FORMAT_DWVW_12, "12 bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DWVW_16, "16 bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DWVW_24, "24 bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DWVW_N, "N bit Delta Width Variable Word encoding");
  hsubtype.insert (SF_FORMAT_DPCM_8, "8 bit differential PCM (XI only)");
  hsubtype.insert (SF_FORMAT_DPCM_16, "16 bit differential PCM (XI only)");
  hsubtype.insert (SF_FORMAT_VORBIS, "Xiph Vorbis encoding");
}


void file_formats_init()
{
  file_formats = new CFileFormats; 
}


void file_formats_done()
{
  delete file_formats;
}


bool CTioPlainAudio::save_16bit_pcm (const QString &fname)
{
  if (! float_input_buffer)
     { 
      qDebug() << "! data";
      return false;
     } 

  SF_INFO sf;

  sf.samplerate = float_input_buffer->samplerate;
  sf.channels = float_input_buffer->channels;
  sf.format = SF_FORMAT_WAV; 
  sf.format |= SF_FORMAT_PCM_16;
  
  if (! sf_format_check (&sf))
     {
      qDebug() << "! sf_format_check (&sf)";
      return false;  
     }  

// qDebug() << "CTioPlainAudio::save_16bit_pcm";

//correctly convert data to 16-bit array

  size_t buflen = float_input_buffer->length_frames * sf.channels;
  
  float *flbuf;
    
  if (float_input_buffer->channels == 1) 
     {
      flbuf = float_input_buffer->buffer[0];
     }
  else  
      flbuf = float_input_buffer->to_interleaved();
  
  short int *buf = new short int [buflen];
    
  for (size_t i = 0; i < buflen; i++)
      {
       float f = flbuf[i];

       if (f >= 1.0)
          buf[i] = f * 32767;
       else
          buf[i] = f * 32768;
       }
   
  SNDFILE *file = sf_open (fname.toUtf8(), SFM_WRITE, &sf);
  
  sf_writef_short (file, buf, float_input_buffer->length_frames);
  
  sf_close (file);

  delete [] buf;
  
  if (float_input_buffer->channels != 1) 
     delete [] flbuf;

  return true;
}


CTioProxy::CTioProxy()
{
  //qDebug() << "CTioProxy::CTioProxy";
  ronly = true;
  id = "CTioProxy";
  
  
 // proxies[exts_mp3] = "lame --decode \"@FILEIN\" \"@FILEOUT\"";
  //extensions += exts_mp3;
  
  QString exts_videos = "mp3,avi,mp4,mpeg2,flv,mkv,aac,vob,wmv,3gp,3ga,m2t,mov,mpeg,h264,ts,webm,asf,wma,ogm,wv,wvc,rm,qt,nut,smi,ac3,divx,dv,fli,flc,cpk";
    
  //QString exts_videos = "avi,mp4";
  
  //FFMPEG: ffmpeg -i \"@FILEIN\" -acodec pcm_s16le -ac 2 \"@FILEOUT\"
 
  //QString handler_ffmpeg = "ffmpeg -y -nostats -i \"@FILEIN\" -acodec pcm_s16le -ac 2 \"@FILEOUT\"";
  
//  QString handler_mplayer = "mplayer -ao pcm:fast:file=\"@FILEOUT\" -vo null -vc null \"@FILEIN\"";
  
  QStringList sl_proxy_video_decoders;

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)


  QString ffmpeg_path = QCoreApplication::applicationDirPath() + "/" + "ffmpeg.exe";
  sl_proxy_video_decoders.append (ffmpeg_path + " -y -nostats -i \"@FILEIN\" -acodec pcm_s16le -ac 2 \"@FILEOUT\"");

  QString mplayer_path = QCoreApplication::applicationDirPath() + "/" + "mplayer.exe";
  sl_proxy_video_decoders.append (mplayer_path + " -really-quiet -ao pcm:fast:file=\"@FILEOUT\" -vo null -vc null \"@FILEIN\"");

#else

  sl_proxy_video_decoders.append ("ffmpeg -y -nostats -i \"@FILEIN\" -acodec pcm_s16le -ac 2 \"@FILEOUT\"");
  sl_proxy_video_decoders.append ("mplayer -really-quiet -ao pcm:fast:file=\"@FILEOUT\" -vo null -vc null \"@FILEIN\"");

#endif
  
  //qDebug() << "=========== proxy_video_decoder: " << proxy_video_decoder; 
    
  proxies[exts_videos] = sl_proxy_video_decoders[proxy_video_decoder];
  
  QStringList lexts_videos = exts_videos.split (",");
  foreach (QString ext, lexts_videos)
          {
           extensions += ext;
          }
}


CTioProxy::~CTioProxy()
{


}


CFloatBuffer* CTioProxy::load (const QString &fname)
{
  QString ext = file_get_ext (fname);

  QString command; 

  QList <QString> keys = proxies.keys();
 
  for (int i = 0; i < keys.size(); i++)
     {
      QString key = keys[i];
      
      if (key.indexOf (",") != -1)
         {
          QStringList l = key.split (",");
          if (l.indexOf (ext) != -1)
             command = proxies[key]; 
         }
      else
          {
           if (key == ext)
              command = proxies[key];
          }   
     }

  if (command.isEmpty() || command.isNull())
    return 0;
  
  command = command.replace ("@FILEIN", fname);
  command = command.replace ("@FILEOUT", temp_mp3_fname);
    
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
  QTime tm;
  tm.start();
  
  int exit_code = QProcess::execute (command);
  
  qDebug() << "elapsed: " << tm.elapsed();
  
  QApplication::restoreOverrideCursor();
  
  if (exit_code < 0)
     return 0;

  if (file_exists (temp_mp3_fname))
     {
      CTioPlainAudio *pa = new CTioPlainAudio (true);
      CFloatBuffer *fb = pa->load (temp_mp3_fname);
      return fb;
     }
}


bool CTioProxy::save (const QString &fname)
{
  error_string = tr ("saving of this format is not supported");
  return false;
}


bool CTioProxy::save_16bit_pcm (const QString &fname)
{
  error_string = tr ("saving of this format is not supported");
  return false;
}
