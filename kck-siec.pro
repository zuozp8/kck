#-------------------------------------------------
#
# Project created by QtCreator 2011-11-27T19:55:08
#
#-------------------------------------------------

QT       += core gui network

TARGET = kck-siec
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    camerathread.cpp

HEADERS  += widget.hpp \
    camerathread.hpp

LIBS += -lopencv_core -lopencv_highgui

