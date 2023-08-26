/**
 * @file
 * Header: Russell Environment
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */


#ifndef RUSSELLENVIRONMENT_H
#define RUSSELLENVIRONMENT_H

#include "environmentclass.h"

//RJG - This is Russell's environmental class which inherets environment class.
class russellenvironment : public EnvironmentClass
{
public:
    russellenvironment(EnvironmentSettings constructorSettings);
    void regenerate();

private:
    struct seed
    {
        double n, m;
        double colour[3];
        double size;
        double nv, mv;
        bool initialised = false;
    };

    seed seeds[1000];

    void doLaplace();

    //Data structures
    //Do it all in double colourMap so don't get errors from using environment (integers)
    QVector<QVector<QVector<double>>> colourMap;
    QVector<QVector<QVector<bool>>> templaplace;
    QVector<QVector<int>> laplace;

    int buffer;
    int nSeed;
    int maxSize;
    int sizeVelocity;
    int maxAcceleration;
    int maxColourVelocity;
    int numbGenerations;

    double converge;
    double factor;
    double maximumVelocity;

    bool periodic;
    bool blur;

    double na, ma;
};


#endif // RUSSELLENVIRONMENT_H
