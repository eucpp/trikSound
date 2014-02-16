#-------------------------------------------------
#
# Project created by QtCreator 2014-02-16T17:55:19
#
#-------------------------------------------------

QT += core

QT -= gui

TARGET = trikSound
TEMPLATE = lib

DEFINES += TRIKSOUND_LIBRARY

CONFIG += mobility
MOBILITY += multimedia

# подключаем pocketsphinx
INCLUDEPATH += /usr/local/include/sphinxbase /usr/local/include/pocketsphinx

INCLUDEPATH += include/ include/filters

LIBS+= -L/usr/local/lib -lpocketsphinx -lsphinxbase

trik {
    message("Building for Trik")

    QT += multimedia

    DEFINES += TRIK

    BUILD_FOLDER = ../trik-build-robotVoice-debug
    DESTDIR = $$BUILD_FOLDER/
    OBJECTS_DIR = $$BUILD_FOLDER/
    MOC_DIR = $$BUILD_FOLDER/
    RCC_DIR = $$BUILD_FOLDER/
    UI_DIR = $$BUILD_FOLDER/
}

SOURCES += \
    src/wavFile.cpp \
    src/soundRecorder.cpp \
    src/recognizer.cpp \
    src/circularBuffer.cpp \
    src/audioProcessor.cpp \
    src/audioBuffer.cpp \
    src/pocketsphinxDecoder.cpp

HEADERS += \
    include/trikSound_global.h \
    include/wavFileTest.h \
    include/wavFile.h \
    include/trikSound_global.h \
    include/soundRecorder.h \
    include/recognizer.h \
    include/circularBuffer.h \
    include/audioProcessor.h \
    include/audioBuffer.h \
    include/pocketsphinxDecoder.h \
    include/filters/audioFilter.h \
    include/audioPipe.h \
    include/filters/soundCaptureFilter.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
