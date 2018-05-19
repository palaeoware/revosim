/**
 * @file
 * Header: Critter
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

#ifndef CRITTER_H
#define CRITTER_H

#include <QtGlobal>

class Critter
{
public:
    Critter();
    void initialise(quint64 gen, quint8 *env, int x, int y, int z, quint64 species);
    int recalc_fitness(quint8 *env);
    int breed_with_parallel(int xpos, int ypos, Critter *partner, int *newgenomecount_local);
    bool iterate_parallel(int *KillCount_local, int addfood);

    int xpos, ypos, zpos;

    quint64 genome;
    quint32 ugenecombo;
    int age; //start off positive - 0 is dead - reduces each time
    int fitness;
    int energy; //breeding energy

    quint64 speciesid; //this is inherited from parents
};


#endif // CRITTER_H
