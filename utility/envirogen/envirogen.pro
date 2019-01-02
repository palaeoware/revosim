#-------------------------------------------------
#
# Project created by QtCreator 2012-03-20T17:38:53
#
#-------------------------------------------------

QT  += core gui widgets concurrent

TARGET = EnviroGen

DESTDIR \
    += \
    bin

TEMPLATE = app

# Load the SPIERS version number
include(../../version.pri)

MOC_DIR += build

OBJECTS_DIR += build

SOURCES += \
    russellenvironment.cpp \
    mainwindow.cpp \
    main.cpp \
    environmentscene.cpp \
    environmentclass.cpp \
    markenvironment.cpp \
    noiseenvironment.cpp \
    combine.cpp \
    colour.cpp \
    makestack.cpp \
    randoms.cpp \
    darkstyletheme.cpp \
    about.cpp

HEADERS  += \
    ui_mainwindow.h \
    ui_dialog.h \
    mainwindow.h \
    environmentscene.h \
    environmentclass.h \
    markenvironment.h \
    noiseenvironment.h \
    combine.h \
    colour.h \
    makestack.h \
    randoms.h \
    russellenvironment.h \
    version.h \
    darkstyletheme.h \
    about.h

FORMS    += \
    mainwindow.ui \
    about.ui

RESOURCES += \
    resources.qrc

#Needed to use C++ lamda functions
CONFIG += c++11
