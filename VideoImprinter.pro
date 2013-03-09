#-------------------------------------------------
#
# Project created by QtCreator 2013-03-04T21:18:35
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoImprinter
TEMPLATE = app


SOURCES += main.cpp\
        videoimprinter.cpp \
    videoplayer.cpp \
    eventeditor.cpp \
    eventmodel.cpp \
    videoevent.cpp

HEADERS  += videoimprinter.h \
    videoplayer.h \
    eventeditor.h \
    eventmodel.h \
    videoevent.h
