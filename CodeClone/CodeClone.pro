TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    clone_detection.cpp

LIBS += -ljsoncpp
LIBS += -fopenmp

HEADERS += \
    clone_detection.h
