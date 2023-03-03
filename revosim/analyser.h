/**
 * @file
 * Header: Analyser
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

#ifndef ANALYSER_H
#define ANALYSER_H

#include "logspecies.h"
#include "sortablegenome.h"
#include "globals.h"
#include "hashablegenome.h"
#include "genomehashtable.h"
#include "groupdata.h"
#include <QColor>
#include <QHash>
#include <QList>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QFuture>
#include <QQueue>
/**
 * @brief The Species class
 */

class Species
{
public:
    Species();

    int internalID;
    int size;
    int genomeDiversity;
    float fitness;
    float envFitness;
    float totalEnergy;
    float totalStolenEnergy;
    int originTime;
    float trophicLevel;
    float Ca, NCa, Cr, NCr;
    LogSpecies *logSpeciesStructure; //which log species it's equivalent to
    quint32 type[MAX_GENOME_WORDS];
    quint64 ID;
    quint64 parent;
    float frequenciesAtOrigination[MAX_GENOME_WORDS][32]; //for gene frequency tracking
    float frequenciesLastIteration[MAX_GENOME_WORDS][32];
    LogSpeciesDataItem complexLogData; //for on-the-fly logging of complex stuff - the logspecies does it per iteration - this keeps one copy, of the most recent version
};

/**
 * @brief The Analyser class
 */
class Analyser
{
public:
    Analyser();
    ~Analyser();

    void groupsGenealogicalTracker();
    void groupsGenealogicalTracker_v3();


    bool speciationLogging;

    //All this no longer needed or should be in logging stuff?
    QString speciationLogText;

private:

    QHash<quint64, GenomeHashTable *> allTables;

    int genomesTotalCount;

    QList<QFuture<void>*> FuturesList;
    QQueue<Species *> speciesQueue;
    int perSpeciesAnalyis(Species *s);
    void gatherGenomesParallel(int firstx, int lastx);
    int generateGroupsFor(GenomeHashTable *thisSpecies);
    void recordFrequencies(Species *sp, GenomeHashTable *spTable, int group, bool firstFind);
    void addSpeciesToList(Species s);
    int getNextSpeciesID();
    void addLogSpecies(quint64 speciesID, LogSpecies *logSpecies);
    QMutex addSpeciesToListMutex, getNextSpeciesIDMutex, addLogSpeciesMutex, speciesQueueMutex, speciationLogTextMutex;
    QList<Species> *newSpeciesList;
    Species *getNextSpeciesToWorkOn();
    void speciesAnalysisWorker(int threadID);
    void doRunningLogs(QHash<qint32, GroupData *> *groups, Species *thisSpecies);
    void reportGroups(QHash<int, GroupData *> *groups);
    int generateGroupsForHeuristic(GenomeHashTable *thisSpecies, int comparisons);
};

#endif // ANALYSER_H
