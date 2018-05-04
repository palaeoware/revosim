/**
 * @file
 * Header: Analyser
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

#ifndef ANALYSER_H
#define ANALYSER_H
#include <QString>
#include <QList>
#include "sortablegenome.h"
#include "logspecies.h"
#include <QHash>
#include <QColor>

class species
{
public:
    species();
    quint64 type;
    quint64 ID;
    int internalID;
    int size;
    quint64 parent;
    int origintime;
    LogSpecies *logspeciesstructure;
};

class Analyser
{
public:
    Analyser();
    void AddGenome_Fast(quint64 genome);
    QString SortedSummary();
    QString Groups();
    int SpeciesIndex(quint64 genome);
    void Groups_With_History_Modal();
    void Groups_2017();

    //New
    QList <quint64> genome_list;
    QList <int> genome_count;
    QList<int> species_id;
    QList<int> lookup_persistent_species_ID;
    int genome_groups[256*256]; //this is static array for speed
    quint64 type_genomes_per_group[256*256];
    int type_genome_groups[256*256];
    int next_type_genome;

private:
    quint8 randtweak(quint8 oldval);
    //Old
    QList <sortablegenome> genomes;
    QList <int>unusedgroups;

    int genomes_total_count;
    int genomes_listsize;
    int Spread(int position, int group);
    int Spread_Fast(int position, int group);
};

#endif // ANALYSER_H
