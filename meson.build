project('eko', ['cpp','c'],
         default_options : ['cpp_std=c++11'],
         version : '7.0.0',
         license : 'GPLv3')

add_global_arguments('-DVERSION_NUMBER="7.0.0"', language : 'cpp')
add_global_arguments('-DUSE_QML_STUFF=1', language : 'cpp')

#harcoded for now
moc_params = ['-DQT_VERSION=0x050000', '-DUSE_QML_STUFF=1']

if build_machine.system() == 'linux'
moc_params += ['-DQ_OS_LINUX=1', '-DQ_OS_UNIX=1']
endif

compiler = meson.get_compiler('cpp')

sndfile_dep = compiler.find_library('sndfile', required : true)
samplerate_dep = compiler.find_library('samplerate', required : true)
portaudio_dep = compiler.find_library('portaudio', required : true)

qt5_dep = dependency('qt5', modules : ['Core', 'Gui', 'Widgets']) 
qt5 = import('qt5') 


eko_headers_moc = [
        'eko.h',
        'document.h',
'utils.h',
'fman.h',
'shortcuts.h',
'logmemo.h',
'tio.h',
'fxset.h',
'gui_utils.h',
'libretta_interpolator.h',
'floatbuffer.h',
'envelope.h',
'fx-filter.h',
'fx-panners.h',
'cvlevelmeter.h',
'fxrack.h',
'afx.h',
'fxlist.h',
'noisegen.h',
'db.h',
'fxpresets.h']
  

src_processed = qt5.preprocess(
                moc_headers : eko_headers_moc,
                moc_extra_arguments: moc_params,
                qresources  : 'eko.qrc')


eko_source = ['main.cpp',
'eko.cpp',
'main.cpp',
'document.cpp',
'utils.cpp',
'fman.cpp',
'shortcuts.cpp',
'logmemo.cpp',
'tio.cpp',
'fxset.cpp',
'gui_utils.cpp',
'libretta_interpolator.cpp',
'floatbuffer.cpp',
'envelope.cpp',
'fx-filter.cpp',
'fx-panners.cpp',
'cvlevelmeter.cpp',
'fxrack.cpp',
'afx.cpp',
'fxlist.cpp',
'noisegen.cpp',
'db.cpp',
'fxpresets.cpp']


eko_exe = executable ('eko',
  sources : [src_processed, eko_source], 
  install : true,
  dependencies : [qt5_dep, sndfile_dep, portaudio_dep, samplerate_dep]
  )


install_data(['icons/eko.png'],
             install_dir : 'share/icons/hicolor/64x64/apps')

install_data(['desktop/eko.desktop'],
             install_dir : 'share/applications')
