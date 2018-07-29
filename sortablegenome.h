/**
 * @file
 * Header: Sortable Genome
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

#ifndef SORTABLEGENOME_H
#define SORTABLEGENOME_H

#include "QtGlobal"

/**
 * @brief The SortableGenome class
 */
class SortableGenome
{
public:
    SortableGenome(quint64 genome, int f, int c);

    bool operator<(const SortableGenome &rhs) const;
    bool operator==(const SortableGenome &rhs) const;
    int fit;
    int group;
    int count;
    quint64 genome;
};

#endif // SORTABLEGENOME_H
