/**
 * @file
 * Header: Sortable Genome
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

#ifndef SORTABLEGENOME_H
#define SORTABLEGENOME_H

#include "QtGlobal"
#include "hashablegenome.h"

class SortableGenome
{
public:
    SortableGenome(quint32 *genome, int length, int f, int c);
    bool operator<(const SortableGenome &rhs) const;
    bool operator==(const SortableGenome &rhs) const;
    int fit;
    int genomeLength;
    quint32 *genome;
    int group;
    int count;
};

#endif // SORTABLEGENOME_H
