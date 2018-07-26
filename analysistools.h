/**
 * @file
 * Header: Analysis Tools
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2018 by Mark Sutton, Russell Garwood,
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
#include <QString>
#include <QMap>
#include <QList>
#include "logspecies.h"

#define ANALYSIS_TOOL_CODE_GENERATE_TREE 0
#define ANALYSIS_TOOL_CODE_RATES_OF_CHANGE 1
#define ANALYSIS_TOOL_CODE_EXTINCT_ORIGIN 2
#define ANALYSIS_TOOL_CODE_STASIS 3
#define ANALYSIS_TOOL_CODE_COUNT_PEAKS 4
#define ANALYSIS_TOOL_CODE_MAKE_NEWICK 5
#define ANALYSIS_TOOL_CODE_DUMP_DATA 6

#define SCALE 100

class logged_species
{
public:
    logged_species();
    quint64 start;
    quint64 end;
    quint64 parent;
    int maxsize;
    quint64 totalsize;
    int occurrences;
    quint64 lastgenome;
    quint64 genomes[SCALE];
    int sizes[SCALE];
    QList <float> avsizes;
    QList <float> avchanges;
};

class stasis_species
{
public:
    stasis_species();
    quint64 ID;
    qint64 start;
    qint64 end;
    QList <quint64> genomes;
    QList <quint64> genome_sample_times;
    QList <float> resampled_average_genome_changes;
};

class AnalysisTools
{
public:
    AnalysisTools();
    bool doesthiscodeneedafile(int code);
    QString GenerateTree(QString filename);
    QString ExtinctOrigin(QString filename);
    QString SpeciesRatesOfChange(QString filename);
    QString Stasis(QString filename, int slot_count, float percentilecut, int qualifyingslotcount);
    QString CountPeaks(int r, int g, int b);

    int find_closest_index(QList <quint64>time_list, float look_for, float slot_width);
    QString MakeNewick(LogSpecies *root, quint64 min_speciessize, bool allowexclude);
    QString DumpData(LogSpecies *root, quint64 min_speciessize, bool allowexclude);
    static QString ReturnBinary(quint64 genome);
private:
    void MakeListRecursive(QList<quint64> *magiclist, QMap <quint64, logged_species> *species_list,
                           quint64 ID, int insertpos);
};


#endif // ANALYSISTOOLS_H
