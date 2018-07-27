/**
 * @file
 * Header: Analysis Tools
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

#ifndef ANALYSISTOOLS_H
#define ANALYSISTOOLS_H

#include "logspecies.h"

#include <QMap>
#include <QList>
#include <QString>

#define ANALYSIS_TOOL_CODE_GENERATE_TREE 0
#define ANALYSIS_TOOL_CODE_RATES_OF_CHANGE 1
#define ANALYSIS_TOOL_CODE_EXTINCT_ORIGIN 2
#define ANALYSIS_TOOL_CODE_STASIS 3
#define ANALYSIS_TOOL_CODE_COUNT_PEAKS 4
#define ANALYSIS_TOOL_CODE_MAKE_NEWICK 5
#define ANALYSIS_TOOL_CODE_DUMP_DATA 6

#define SCALE 100

class LoggedSpecies
{
public:
    LoggedSpecies();

    int maxSize;
    int occurrences;
    int sizes[SCALE];
    QList<float> averageSizes;
    QList<float> averageChanges;

    quint64 start;
    quint64 end;
    quint64 parent;
    quint64 totalSize;
    quint64 lastGenome;
    quint64 genomes[SCALE];
};

class StasisSpecies
{
public:
    StasisSpecies();

    quint64 ID;
    qint64 start;
    qint64 end;
    QList<quint64> genomes;
    QList<quint64> genomeSampleTimes;
    QList<float> resampledAverageGenomeChanges;
};

class AnalysisTools
{
public:
    AnalysisTools();

    static QString returnBinary(quint64 genome);

    bool dataFileNeededCheck(int code);
    int findClosestIndex(QList <quint64>timeList, float lookFor, float slotWidth);
    QString generateTree(QString filename);
    QString extinctOrigin(QString filename);
    QString speciesRatesOfChange(QString filename);
    QString stasis(QString filename, int slotCount, float percentileCut, int qualifyingSlotCount);
    QString countPeaks(int r, int g, int b);
    QString makeNewick(LogSpecies *root, quint64 minSpeciesSize, bool allowExclude);
    QString dumpData(LogSpecies *root, quint64 minSpeciesSize, bool allowExclude);

private:
    void makeListRecursive(QList<quint64> *magicList, QMap <quint64, LoggedSpecies> *speciesList,
                           quint64 ID, int insertPosition);
};

#endif // ANALYSISTOOLS_H
