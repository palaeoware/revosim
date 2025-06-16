# -------------------------------------------------
# Project created by QtCreator 2010-02-10T13:30:03
# -------------------------------------------------

QT += core widgets concurrent gui core5compat

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
    bitcountsystem.cpp \
    breedsystem.cpp \
    cellsettings.cpp \
    custommemorymanager.cpp \
    environmentfitnesssystem.cpp \
    genomecomparisonsystem.cpp \
    genomehashtable.cpp \
    groupdata.cpp \
    hashablegenome.cpp \
    imagesequence.cpp \
    interactionsystem.cpp \
    linkageclass.cpp \
    linkagesdialog.cpp \
    logsimulation.cpp \
    mainwindow.cpp \
    mutationsystem.cpp \
    neuralnet.cpp \
    neuralnetweightssystem.cpp \
    neuron.cpp \
    neuron_input.cpp \
    neuron_output.cpp \
    neuron_relu.cpp \
    pathogenssystem.cpp \
    randoms.cpp \
    simmanager.cpp \
    critter.cpp \
    populationscene.cpp \
    environmentscene.cpp \
    simsettings.cpp \
    sortablegenome.cpp \
    analyser.cpp \
    genomecomparison.cpp \
    resizecatcher.cpp \
    analysistools.cpp \
    reseed.cpp \
    logspecies.cpp \
    logspeciesdataitem.cpp \
    about.cpp \
    darkstyletheme.cpp \
    speciesbinentry.cpp \
    speciesidsystem.cpp \
    system.cpp \
    test.cpp \
    variablebreedsystem.cpp \
    variablemutatesystem.cpp \
    visualisationsystem.cpp

HEADERS += mainwindow.h \
    bitcountsystem.h \
    breedsystem.h \
    cellsettings.h \
    custommemorymanager.h \
    environmentfitnesssystem.h \
    genomecomparisonsystem.h \
    genomehashtable.h \
    groupdata.h \
    hashablegenome.h \
    imagesequence.h \
    interactionsystem.h \
    linkageclass.h \
    linkagesdialog.h \
    logsimulation.h \
    mutationsystem.h \
    neuralnet.h \
    neuralnetweightssystem.h \
    neuron.h \
    neuron_input.h \
    neuron_output.h \
    neuron_relu.h \
    pathogenssystem.h \
    randoms.h \
    simmanager.h \
    critter.h \
    populationscene.h \
    environmentscene.h \
    simsettings.h \
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
    globals.h \
    speciesbinentry.h \
    speciesidsystem.h \
    system.h \
    test.h \
    variablebreedsystem.h \
    variablemutatesystem.h \
    visualisationsystem.h

FORMS += mainwindow.ui \
    genomecomparison.ui \
    linkagesdialog.ui \
    reseed.ui \
    about.ui

OTHER_FILES += \
    README.TXT

RESOURCES += \
    resources.qrc

#Needed to use C++ lamda functions
CONFIG += c++17

DISTFILES += \
    ../LICENSE \
    ../README.md \
    .astylerc

RC_FILE = resources/revosim.rc

#Mac icon
ICON = ../resources/revosim.icns

