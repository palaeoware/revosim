/**
 * @file
 * Header: Simulation Manager
 *
 * Provides globals for the simulation - lookup tables for example
 * Only one instance. Single instance - which is the only true global
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
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
#include "critter.h"
#include "logspecies.h"

#include <QFuture>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QMutex>
#include <QPlainTextEdit>
#include <QStringList>
#include <QtConcurrentRun>
#include <QTime>

#define RAND_SEED 10000
#define PREROLLED_RANDS 60000
#define MAX_GENOME_COUNT 100000 //hopefully big enough for all species
#define GRID_X 256
#define GRID_Y 256
#define SLOTS_PER_GRID_SQUARE 256
#define SPECIES_MODE_NONE 0
#define SPECIES_MODE_BASIC 1
#define SPECIES_MODE_PHYLOGENY 2
#define SPECIES_MODE_PHYLOGENY_AND_METRICS 3
#define ENV_MODE_STATIC 0
#define ENV_MODE_ONCE 1
#define ENV_MODE_LOOP 2
#define ENV_MODE_BOUNCE 3

// Settable ints
extern int gridX;
extern int gridY;
extern int slotsPerSquare;
extern int startAge;
extern int target;
extern int settleTolerance;
extern int dispersal;
extern int food;
extern int breedThreshold;
extern int breedCost;
extern int maxDifference;
extern int mutate;
extern quint64 reseedGenome;
extern int nextEnvironmentChange;
extern int environmentChangeCounter;
extern quint8 environmentMode;

// Settable bools
extern bool recalculateFitness;
extern bool asexual;
extern bool sexual;
extern bool nonspatial;
extern bool toroidal;
extern bool breedSpecies;
extern bool breedDifference;
extern bool logging;
extern bool gui;
extern bool speciesLogging;
extern bool speciesLoggingToFile;
extern bool reseedKnown;
extern bool environmentChangeForward;
extern bool environmentInterpolate;
extern bool allowExcludeWithDescendants;

extern quint32 tweakers[32]; // the 32 single bit XOR values (many uses!)
extern quint64 tweakers64[64]; // 64-bit versions
extern quint32 bitCounts[65536]; // the bytes representing bit count of each number 0-635535
extern quint32 xorMasks[256][3]; //determine fitness - just 32 bit (lower bit of genome)
extern int dispersalX[256][256];
extern int dispersalY[256][256];
extern quint64 geneX[65536];
extern int nextGeneX;
extern quint8 probabilityBreed[65536][16];

// Randoms
extern quint8 randoms[65536];
extern quint16 nextRandom;

// Globabl data
extern Critter critters[GRID_X][GRID_Y][SLOTS_PER_GRID_SQUARE]; //main array - static for speed
extern quint8 environment[GRID_X][GRID_Y][3];  //0 = red, 1 = green, 2 = blue
extern quint8 environmentLast[GRID_X][GRID_Y][3];  //Used for interpolation
extern quint8 environmentNext[GRID_X][GRID_Y][3];  //Used for interpolation
extern quint32 totalFittness[GRID_X][GRID_Y]; // Sum fitness critters in each square
extern quint64 iteration;

//These next to hold the babies... old style arrays for max speed
extern quint64 newGenomes[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint32 newGenomeX[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint32 newGenomeY[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern int newGenomeDispersal[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern quint64 newGenomeSpecies[GRID_X * GRID_Y * SLOTS_PER_GRID_SQUARE * 2];
extern int newGenomeCount;

extern int environmentChangeRate;
extern int speciesSamples; // no longer used - keep for backwards compat of files
extern int speciesSensitivity; // no longer used - keep for backwards compat of files
extern int timeSliceConnect; // no longer used - keep for backwards compat of files

extern QString speciesLoggingFile;
extern bool fitnessLoggingToFile;
extern QString fitnessLoggingFile;


// File handling
extern QStringList environmentFiles;
extern int currentEnvironmentFile;
extern quint64 lastSpeciesCalculated;
extern quint64 lastReport;

// Analysis
extern int breedAttempts[GRID_X][GRID_Y]; // for analysis purposes
extern int breedFails[GRID_X][GRID_Y]; // for analysis purposes
extern int settles[GRID_X][GRID_Y]; // for analysis purposes
extern int settleFails[GRID_X][GRID_Y]; // for analysis purposes
extern int maxUsed[GRID_X][GRID_Y]; // number of slots used within each grid square
extern int aliveCount;
extern int totalRecombination;

// Species stuff
extern QList<Species> oldSpeciesList;
extern QList< QList<Species> > archivedSpeciesLists;
extern quint64 nextSpeciesID;
extern LogSpecies *rootSpecies;
extern QHash<quint64, LogSpecies *> logSpeciesByID;
extern QList<uint> speciesColours;
extern quint8 speciesMode;
extern quint64 minSpeciesSize;
extern quint64 ids;

extern QMutex *mutexes[GRID_X][GRID_Y];

/**
 * @brief The SimManager class
 */
class SimManager
{
public:
    SimManager();

    void setupRun();
    void testcode();
    void loadEnvironmentFromFile(int emode);
    bool iterate(int emode, bool interpolate);
    bool regenerateEnvironment(int emode, bool interpolate);
    int iterateParallel(int firstX, int lastX, int newGenomesLocal, int *killCountLocal);
    int portableRandom();
    int settleParallel(int newGenomeCountsStart, int newGenomeCountsEnd, int *tryCountLocal, int *settleCountLocal, int *birthCountsLocal);

    int warningCount;
    quint8 random8();
    quint32 random32();
    quint64 random64();

private:
    void makeLookups();
    void debugGenome(quint64 genome);

    int processorCount;
    QList<QFuture<int>*> futuresList;

};

extern SimManager *simulationManager;

#define PORTABLE_RAND_MAX 32767

#endif // SIMMANAGER_H
