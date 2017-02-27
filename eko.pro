VERSION = 5.3.1
os2: {
DEFINES += 'VERSION_NUMBER=\'"5.3.1"\''
} else: {
  DEFINES += 'VERSION_NUMBER=\\\"$${VERSION}\\\"'
}


useclang{
    message ("Clang enabled")
    QMAKE_CC=clang
    QMAKE_CXX=clang
    QMAKE_CXXFLAGS += -std=c++11
}


useopencl{
    message ("OpenCL enabled")
    LIBS+= -lOpenCL
    QMAKE_CXXFLAGS += -std=c++0x
    PKGCONFIG += OpenCL
    DEFINES += OPENCL_ENABLE
}

SOURCES += eko.cpp \
    main.cpp \
    document.cpp \
    utils.cpp \
    fman.cpp \
    shortcuts.cpp \
    logmemo.cpp \
    tio.cpp \
    fxset.cpp \
    gui_utils.cpp \
    libretta_interpolator.cpp \ 
    floatbuffer.cpp \
    envelope.cpp \
    fx-filter.cpp \
    fx-panners.cpp \
    cvlevelmeter.cpp \
    fxrack.cpp \
    afx.cpp \
    fxlist.cpp \
    noisegen.cpp \
    db.cpp \
    fxpresets.cpp
#    tinyplayer.cpp

HEADERS += eko.h \
    document.h \
    utils.h \
    fman.h \
    shortcuts.h \
    logmemo.h \
    tio.h \
    fxset.h \
    gui_utils.h \
    libretta_interpolator.h \
    floatbuffer.h \
    envelope.h \
    fx-filter.h \
    fx-panners.h \
    cvlevelmeter.h \
    fxrack.h \
    afx.h \
    fxlist.h \
    noisegen.h \
    db.h \
    fxpresets.h
#    tinyplayer.h



TEMPLATE = app

CONFIG += warn_on \
    thread \
    qt \
    release \
    link_pkgconfig

QT += core
QT += gui

greaterThan(QT_MAJOR_VERSION, 4) {
       QT += widgets
   } else {
#QT += blah blah blah
   }

QMAKE_CXXFLAGS += -std=c++11

#QMAKE_CXXFLAGS += -fpermissive

unix: {

      PKGCONFIG += sndfile \
                   samplerate \
                   portaudio-2.0


#    QMAKE_CXXFLAGS+=-fopenmp
#    LIBS+=-lgomp


       }


isEmpty(PREFIX):PREFIX = /usr/local #path to install
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX

TARGET = eko
target.path = $$BINDIR

INSTALLS += target
RESOURCES += eko.qrc
TRANSLATIONS += translations/eko_ru.ts \
                translations/eko_cs.ts

DISTFILES += ChangeLog \
    COPYING \
    README \
    NEWS \
    NEWS-RU \
    AUTHORS \
    TODO \
    INSTALL \
    icons/* \
    palettes/* \
    manuals/en.html \
    manuals/ru.html \
    translations/* \
    themes/Cotton/stylesheet.css \
    themes/Plum/stylesheet.css \
    themes/Smaragd/stylesheet.css \
    themes/TEA/stylesheet.css \
    themes/Turbo/stylesheet.css \
    themes/Vegan/stylesheet.css



win32: {

#        CONFIG += console 

             exists ("c:\\Qt\\Qt5.3.1\\5.3\\mingw482_32\\include\portaudio.h")
                    {
                     message ("Portaudio FOUND")
                     LIBS += -llibportaudio-2
                    }


                    exists ("c:\\Qt\\Qt5.3.1\\5.3\\mingw482_32\\include\sndfile.h")
                    {
                     message ("libsndfile FOUND")
                     LIBS += -llibsndfile-1
                    }

                    exists ("c:\\Qt\\Qt5.3.1\\5.3\\mingw482_32\\include\samplerate.h")
                    {
                     message ("libsamplerate FOUND")
                     LIBS += -llibsamplerate-0
                    }


       }
