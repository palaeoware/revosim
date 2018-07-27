/**
 * @file
 * Header: Analyser
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

#ifndef ANALYSER_H
#define ANALYSER_H

#include "sortablegenome.h"
#include "logspecies.h"

#include <QColor>
#include <QHash>
#include <QList>
#include <QString>

class Species
{
public:
    Species();

    int internalID;
    int size;
    int originTime;
    LogSpecies *logSpeciesStructure;
    quint64 type;
    quint64 ID;
    quint64 parent;
};

class Analyser
{
public:
    Analyser();

    void addGenomeFast(quint64 genome);
    void groupsWithHistoryModal();
    void groupsGenealogicalTracker();
    int speciesIndex(quint64 genome);

    QList<quint64> genomeList;
    QList<int> genomeCount;
    QList<int> speciesID;
    QList<int> lookupPersistentSpeciesID;

private:
    int genomesTotalCount;
    QList<sortablegenome> genomes;
};

#endif // ANALYSER_H
