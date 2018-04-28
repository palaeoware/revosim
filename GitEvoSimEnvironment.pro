#-------------------------------------------------
#
# Project created by QtCreator 2012-03-20T17:38:53
#
#-------------------------------------------------

QT       += core gui widgets concurrent

TARGET = GitEvoSimEnvironment
TEMPLATE = app


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
    darkstyletheme.cpp

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
    darkstyletheme.h

FORMS    += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

#Needed to use C++ lamda functions
CONFIG += c++11

