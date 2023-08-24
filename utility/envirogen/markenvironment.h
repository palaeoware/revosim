/**
 * @file
 * Header: Mark Environment
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


#ifndef MARKENVIRONMENT_H
#define MARKENVIRONMENT_H

#include <QColor>
#define MAXOBJ 1000

#include "environmentclass.h"
#include "environmentsettings.h"

class markenvironment : public EnvironmentClass
{
public:
    markenvironment(EnvironmentSettings constructorSettings);
    void regenerate();

private:
    quint8 add_and_limit(int oldvalue, int color, double dist, double maxdist, double tightness);
    double objxpos[MAXOBJ];
    double objypos[MAXOBJ];
    double objxvel[MAXOBJ];
    double objyvel[MAXOBJ];
    double objsize[MAXOBJ];
    double objsizeVelocity[MAXOBJ];
    double objcolours[MAXOBJ][3];
    double objcolvel[MAXOBJ][3];
    double objtightness[MAXOBJ];
    double objtightvel[MAXOBJ];
    int objectcount;
    double maxSize, minsize;
    double maximumVelocity, maxsizeVelocity, maxcolvel, maxtightvel, maxtight, mintight, speedfactor;
    double veltweak, sizetweak, coltweak, tighttweak;
    int iter_to_accel;
    int iter_reset;
};


#endif // MARKENVIRONMENT_H
