#-------------------------------------------------
# EnviroGen project
# Project created by QtCreator 2012-03-20T17:38:53
#-------------------------------------------------

QT  += core gui widgets concurrent

TARGET = envirogen

DESTDIR \
    += \
    bin

TEMPLATE = app

# Load the version number
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
    darkstyletheme.h \
    about.h \
    globals.h

FORMS    += \
    mainwindow.ui \
    about.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    ../../LICENSE \
    ../../README.md \
    .astylerc

#Needed to use C++ lamda functions
CONFIG += c++11

#Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie

RC_FILE = resources/envirogen.rc

#Mac icon
ICON = revosim.icns
