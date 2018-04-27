/**
 * @file
 * Header: Log Spieces Data Item
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

#ifndef LOGSPECIESDATAITEM_H
#define LOGSPECIESDATAITEM_H
#include <QtGlobal>

class LogSpeciesDataItem
{
public:
    LogSpeciesDataItem();
    quint64 generation;
    quint64 sample_genome;
    quint32 size; //number of critters
    quint32 genomic_diversity; //number of genomes
    quint16 cells_occupied; //number of cells found in - 1 (as real range is 1-65536, to fit in 0-65535)
    quint8 geographical_range; //max distance between outliers
    quint8 centroid_range_x; //mean of x positions
    quint8 centroid_range_y; //mean of y positions
    quint16 mean_fitness; //mean of all critter fitnesses, stored as x1000
    quint8 min_env[3]; //min red, green, blue found in
    quint8 max_env[3]; //max red, green, blue found in
    quint8 mean_env[3]; //mean environmental colour found in
};

#endif // LOGSPECIESDATAITEM_H
