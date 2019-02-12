# -------------------------------------------------
# Project created by QtCreator 2010-02-10T13:30:03
# -------------------------------------------------
QT += widgets concurrent

TARGET = revosim

DESTDIR \
    += \
    bin

TEMPLATE = app

# Load the version number
include(../version.pri)

MOC_DIR += build

OBJECTS_DIR += build

SOURCES += main.cpp \
    mainwindow.cpp \
    simmanager.cpp \
    critter.cpp \
    populationscene.cpp \
    environmentscene.cpp \
    sortablegenome.cpp \
    analyser.cpp \
    genomecomparison.cpp \
    resizecatcher.cpp \
    analysistools.cpp \
    reseed.cpp \
    logspecies.cpp \
    logspeciesdataitem.cpp \
    about.cpp \
    darkstyletheme.cpp

HEADERS += mainwindow.h \
    simmanager.h \
    critter.h \
    populationscene.h \
    environmentscene.h \
    sortablegenome.h \
    analyser.h \
    genomecomparison.h \
    resizecatcher.h \
    analysistools.h \
    reseed.h \
    logspecies.h \
    logspeciesdataitem.h \
    about.h \
    darkstyletheme.h \
    globals.h

FORMS += mainwindow.ui \
    genomecomparison.ui \
    reseed.ui \
    about.ui

OTHER_FILES += \
    README.TXT

RESOURCES += \
    resources.qrc

#Needed to use C++ lamda functions
CONFIG += c++11

#Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie

DISTFILES += \
    ../LICENSE \
    ../README.md \
    .astylerc

RC_FILE = resources/revosim.rc

