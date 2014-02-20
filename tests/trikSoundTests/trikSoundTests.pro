#-------------------------------------------------
#
# Project created by QtCreator 2014-02-20T16:55:11
#
#-------------------------------------------------

QT       += core testlib

QT       -= gui

TARGET = trikSoundTests
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += mobility
MOBILITY += multimedia

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

TEMPLATE = app

INCLUDEPATH+= ../../include/

LIBS+= -L/usr/lib -ltrikSound

SOURCES += main.cpp

HEADERS += \
    wavFileTest.h \
    circularBufferTest.h
