#-------------------------------------------------
#
# Project created by QtCreator 2016-02-08T16:01:53
#
#-------------------------------------------------

QT  += widgets svg
QT  += core gui printsupport concurrent
QT  += charts webenginewidgets
#QT  += webkitwidgets

TARGET = ThermoFunDemoGUI
TEMPLATE = app

CONFIG += thread
CONFIG += c++11
CONFIG += warn_on

DEFINES += FROM_SRC

!win32 {
  DEFINES += __unix
}

macx-g++ {
  DEFINES += __APPLE__
}

# Define the directory where source code is located
#THRIFT_DIR    = ./thrift
FUN_DIR       = ../thermofun
FUN_CPP       = $$FUN_DIR
FUN_H         = $$FUN_DIR
GUI_DIR       = ./src
GUI_CPP       = $$GUI_DIR
GUI_H         = $$GUI_DIR
CLIENT_DIR = ../DBClient
CLIENT_CPP = $$CLIENT_DIR
CLIENT_H   = $$CLIENT_DIR
#GUI_CPP       =  ./GUI
#GUI_H         =  $$GUI_CPP

DEPENDPATH   += $$GUI_H
DEPENDPATH   += $$GUI_DIR

INCLUDEPATH   += $$GUI_H
INCLUDEPATH   += $$GUI_DIR

DEPENDPATH   += $$FUN_H
DEPENDPATH   += $$FUN_DIR

INCLUDEPATH   += $$FUN_H
INCLUDEPATH   += $$FUN_DIR

DEPENDPATH   += $$CLIENT_H
DEPENDPATH   += $$CLIENT_DIR

INCLUDEPATH   += $$CLIENT_H
INCLUDEPATH   += $$CLIENT_DIR

INCLUDEPATH   += "/usr/local/include"
DEPENDPATH   += "/usr/local/include"
LIBPATH += "/usr/local/lib/"

#temporary
macx-clang {
  DEFINES += __APPLE__
  CONFIG -= warn_on
  CONFIG += warn_off
  LIBS += -llua
}
else{
  LIBS += -llua5.3
}

# Define the directory where CuteMarkEd code is located
#INCLUDEPATH   += "/usr/local/include/app-static"
#DEPENDPATH   += "/usr/local/include/app-static"

LIBS +=  -ljsonui -ljsonio -ljsonimpex
LIBS +=  -lyaml-cpp  -lpugixml
LIBS +=  -lboost_regex -lboost_system -lboost_filesystem
#LIBS += -lapp-static -lhunspell -lmarkdown
LIBS +=  -lcurl  -lvelocypack -lthrift
LIBS += -llua5.3


MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR   = obj

#include($$THRIFT_DIR/thrift.pri)
#include($$GEN_DIR/generator.pri)
include($$GUI_DIR/ThermoFunGUI.pri)
include($$CLIENT_DIR/ThermoDBClient.pri)
include($$FUN_DIR/ThermoFun.pri)

RESOURCES += \
    ThermoFunDemoGUI.qrc

#thrift -r -v --gen cpp

SOURCES += \
    demo/main.cpp \
    demo/ThermoFunMainWindow.cpp

HEADERS += \
    demo/ThermoFunMainWindow.h

FORMS += \
    demo/ThermoFunMainWindow.ui

