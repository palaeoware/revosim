# -------------------------------------------------
# Project created by QtCreator 2010-02-10T13:30:03
# -------------------------------------------------
QT += widgets concurrent
TARGET = revosim
TEMPLATE = app
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
    version.h \
    reseed.h \
    logspecies.h \
    logspeciesdataitem.h \
    about.h \
    darkstyletheme.h
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

DISTFILES += \
    LICENSE.md

