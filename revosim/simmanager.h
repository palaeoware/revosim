/**
 * @file
 * Header: Simulation Manager
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef SIMMANAGER_H
#define SIMMANAGER_H

#include "analyser.h"
#include "bitcountsystem.h"
#include "breedsystem.h"
#include "cellsettings.h"
#include "critter.h"
#include "environmentfitnesssystem.h"
#include "globals.h"
#include "imagesequence.h"
#include "interactionsystem.h"
#include "linkageclass.h"
#include "logsimulation.h"
#include "logspecies.h"
#include "mutationsystem.h"
#include "pathogenssystem.h"
#include "randoms.h"
#include "simsettings.h"
#include "speciesidsystem.h"
#include "variablebreedsystem.h"
#include "variablemutatesystem.h"
#include "visualisationsystem.h"
#include "genomecomparisonsystem.h"
#include "hgtsystem.h"
#include "variablehgtlensystem.h"
#include "variablehgtprobsystem.h"

#include <QElapsedTimer>
#include <QFuture>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QMutex>
#include <QPlainTextEdit>
#include <QStringList>
#include <QtConcurrentRun>
#include <QTime>
#include <QTimer>

#include <QDebug>

/*!
  * Provides globals for the simulation - lookup tables for example. Only one instance of each - which is the only true global
*/



/****** Global lookups ******/
extern int xdisp[256][256];
extern int ydisp[256][256];
extern int nextgenex;
extern quint8 randoms8[65536];
extern quint32 bitCounts[65536]; // the bytes representing bit count of each number 0-635535
extern quint32 genex[65536];
extern quint32 tweakers[32]; // the 32 single bit XOR values (many uses!)
extern quint64 tweakers64[64]; // 64-bit versions
extern qint16 interaction[256][256]; // interactions tables
extern int Randcelllist1[GRID_X];
extern int Randcelllist2[GRID_Y];

/****** Global data ******/
extern Critter critters[GRID_X][GRID_Y][SLOTS_PER_GRID_SQUARE]; //Main array - static for speed
extern quint32 totalFitness[GRID_X][GRID_Y]; // RJG - Sum fitness critters in each square

//These next to hold the babies... old style arrays for max speed
extern int newgenomeDisp[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint32 newgenomeX[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint32 newgenomeY[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint32 *newgenomes[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint64 newgenomespecies[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint64 newgenometrophic[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];

/****** Analysis data lookups ******/
extern int breedAttempts[GRID_X][GRID_Y]; //for analysis purposes
extern int breedSuccess[GRID_X][GRID_Y]; //for analysis purposes
extern int breedGeneration[GRID_X][GRID_Y]; //for analysis purposes
extern int breedFails[GRID_X][GRID_Y]; //for analysis purposes
extern int settles[GRID_X][GRID_Y]; //for analysis purposes
extern int settlefails[GRID_X][GRID_Y]; //for analysis purposes
extern int maxUsed[GRID_X][GRID_Y]; //RJG - number of slots used within each grid square

/****** Species system variables ******/
extern QList<Species> *oldSpeciesList, *newSpeciesList;
extern QList< QList<Species> > archivedSpeciesLists;
extern quint64 nextSpeciesID;
extern LogSpecies *rootSpecies;
extern QHash<quint64, LogSpecies *> logSpeciesByID;
extern QList<uint> speciesColours;
extern quint8 speciesMode;
extern bool allowExcludeWithDescendants;
extern quint64 ids;


class SimManager
{
public:
    SimManager();

    SimSettings *simulationSettings; //RJG - These are the settings which can't be modified per cell, but apply to whole simulation
    CellSettings *cellSettingsMaster; //RJG - These are the overall settings for cells which can be modified by images
    CellSettings cellSettings[GRID_X][GRID_Y]; //RJG - These are derived from above prior to each simulation iteration
    randoms *simulationRandoms;//RJG - These are the randoms REvoSim uses built off QRandomGenerator
    ImageSequence *env; //RJG - this is the environment for the run
    friend class test; //RJG - make friend a test class so it can access private functions to test them

    quint16 ProcessorCount;
    void SetProcessorCount(int count);
    void setupRun();
    bool iterate(int eMode, bool interpolate);

    //---- RJG - this is used to give users a heads up when species system is being slow
    int warningCount;
    quint64 iteration;
    int aliveCount;

    //RJG - Linkages stored as a hash
    QHash <QString, linkageClass> linkages;
    QStringList linkedVariables = {LINKAGES_LIST};

    void makeInteractionLookups();
    QString printGenome(quint64 genome);
    QString printGenome(quint32 genome);

    //Systems
    QList <System *> systemsList;
    EnvironmentFitnessSystem *environmentalFitnessSytem;
    BitCountSystem *bitCountSystem;
    BreedSystem *breedSystem;
    VariableMutateSystem *variableMutateSystem;
    MutationSystem *mutationSystem;
    VariableBreedSystem *variableBreedSystem;
    PathogensSystem *pathogensSystem;
    SpeciesIDSystem *speciesIDSytem;
    InteractionSystem *interactionSystem;
    VisualisationSystem *visualisationSystem;
    VisualisationSystem *visualisationSystem2;
    LogSimulation *simulationLog;
    GenomeComparisonSystem *genomeComparisonSystem;
    HgtSystem *hgtSystem;
    VariableHgtProbSystem *variableHgtProbSystem;
    VariableHgtLenSystem *variableHgtLenSystem;




private:
    void makeLookups();
    void populateCellSettings();
    QList<QFuture<int>*> FuturesList;
    int iterateParallel(int firstx, int lastx, int newgenomes_local, int *killCountLocal);
    int settleParallel(int newGenomeCountsStart, int newGenomeCountsEnd, int *birthCountsLocal);

    bool temp_path_on;

};

extern SimManager *simulationManager;

#endif // SIMMANAGER_H
