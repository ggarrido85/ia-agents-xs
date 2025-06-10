#-------------------------------------------------
#
# Project created by QtCreator 2013-04-01T10:54:35
#
#-------------------------------------------------

QT       += core xml sql

QT       -= gui

TARGET = Agentes
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += cachemanager.cpp \
    typeobjective.cpp \
    dataobjective.cpp \
    chequeocodigo.cpp \
    seobjetivos.cpp \
    utils.cpp \
    tokenizer.cpp \
    polaca.cpp \
    connectionsettings.cpp \
    postgreconexion.cpp

HEADERS += \
    cachemanager.h \
    typeobjective.h \
    dataobjective.h \
    chequeocodigo.h \
    seobjetivos.h \
    utils.h \
    tokenizer.h \
    polaca.h \
    connectionsettings.h \
    postgreconexion.h

#config_principal{
   SOURCES += main.cpp
#}

#include(Test/Prueba_1/test.pri)
