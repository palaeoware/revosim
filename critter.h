/**
 * @file
 * Header: Critter
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

#ifndef CRITTER_H
#define CRITTER_H

#include <QtGlobal>

class Critter
{
public:
    Critter();

    void initialise(quint64 generation, quint8 *environment, int x, int y, int z, quint64 species);
    bool iterateParallel(int *killCountLocal, int addFood);
    int recalculateFitness(const quint8 *environment);
    int breedWithParallel(int xPosition, int yPosition, Critter *partner, int *newGenomeCountLocal);

    int xPosition{};
    int yPosition{};
    int zPosition{};
    int age; //start off positive - 0 is dead - reduces each time
    int fitness;
    int energy; //breeding energy
    quint64 genome;
    quint64 speciesID; //this is inherited from parents
};

#endif // CRITTER_H
