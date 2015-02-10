#-------------------------------------------------
#
# Project created by QtCreator 2015-02-07T21:59:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TimeToRelax
TEMPLATE = app

CONFIG += static

win32 {
    RC_ICONS = ../resources/icons/app.ico
}
unix {
}

#static {
#    CONFIG += static
#    QTPLUGIN += qsqloci
#    DEFINES += STATIC
#    message("Static build.")
#}

QMAKE_CXXFLAGS += -std=c++11

HEADERS  += \
    context_menu.h \
    app.h \
    icon.h \
    config.h

SOURCES += main.cpp\
    context_menu.cpp \
    app.cpp \
    icon.cpp \
    config.cpp

RESOURCES += \
    ../resources/recources.qrc
